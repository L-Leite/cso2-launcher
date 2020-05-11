#pragma once

#include "containers/constructor_helpers.hpp"
#include "containers/utlmemory.hpp"

#include <cstring>

template <class I>
struct UtlRBTreeLinks_t
{
    I m_Left;
    I m_Right;
    I m_Parent;
    I m_Tag;
};

template <class T, class I>
struct UtlRBTreeNode_t : public UtlRBTreeLinks_t<I>
{
    T m_Data;
};

template <class T, typename I = unsigned short,
          typename L = bool ( * )( const T&, const T& ),
          class M = CUtlMemory<UtlRBTreeNode_t<T, I>, I>>
class CUtlRBTree
{
public:
    enum NodeColor_t
    {
        RED = 0,
        BLACK
    };

    using LessFunc_t = L;

    using Node_t = UtlRBTreeNode_t<T, I>;
    using Links_t = UtlRBTreeLinks_t<I>;

    LessFunc_t m_LessFunc;

    M m_Elements;
    I m_Root;
    I m_NumElements;
    I m_FirstFree;
    typename M::Iterator_t m_LastAlloc;  // the last index allocated

    Node_t* m_pElements;

    //-----------------------------------------------------------------------------
    // constructor, destructor
    //-----------------------------------------------------------------------------
    CUtlRBTree( int growSize = 0, int initSize = 0,
                const LessFunc_t& lessfunc = 0 )
        : m_LessFunc( lessfunc ), m_Elements( growSize, initSize ),
          m_Root( InvalidIndex() ), m_NumElements( 0 ),
          m_FirstFree( InvalidIndex() ),
          m_LastAlloc( m_Elements.InvalidIterator() )
    {
        ResetDbgInfo();
    }

    CUtlRBTree( const LessFunc_t& lessfunc )
        : m_Elements( 0, 0 ), m_LessFunc( lessfunc ), m_Root( InvalidIndex() ),
          m_NumElements( 0 ), m_FirstFree( InvalidIndex() ),
          m_LastAlloc( m_Elements.InvalidIterator() )
    {
        ResetDbgInfo();
    }

    inline ~CUtlRBTree() { Purge(); }

    void SetLessFunc( const LessFunc_t& func )
    {
        if ( !m_LessFunc )
        {
            m_LessFunc = func;
        }
    }

    //-----------------------------------------------------------------------------
    // Invalid index
    //-----------------------------------------------------------------------------

    static I InvalidIndex() { return (I)M::InvalidIndex(); }

    void ResetDbgInfo() { m_pElements = (Node_t*)m_Elements.Base(); }

    inline Links_t& Links( I i ) { return *(Links_t*)&m_Elements[i]; }
    inline const Links_t& Links( I i ) const
    {
        // Sentinel node, makes life easier
        static Links_t s_Sentinel = { InvalidIndex(), InvalidIndex(),
                                      InvalidIndex(),
                                      CUtlRBTree<T, I, L, M>::BLACK };

        return ( i != InvalidIndex() ) ? *(Links_t*)&m_Elements[i] :
                                         *(Links_t*)&s_Sentinel;
    }

    //-----------------------------------------------------------------------------
    // Gets the children
    //-----------------------------------------------------------------------------

    inline I Parent( I i ) const { return Links( i ).m_Parent; }
    inline I LeftChild( I i ) const { return Links( i ).m_Left; }
    inline I RightChild( I i ) const { return Links( i ).m_Right; }

    //-----------------------------------------------------------------------------
    // Sets the children
    //-----------------------------------------------------------------------------

    inline void SetParent( I i, I parent ) { Links( i ).m_Parent = parent; }
    inline void SetLeftChild( I i, I child ) { Links( i ).m_Left = child; }
    inline void SetRightChild( I i, I child ) { Links( i ).m_Right = child; }

    //-----------------------------------------------------------------------------
    // Tests if a node is a left or right child
    //-----------------------------------------------------------------------------

    inline bool IsLeftChild( I i ) const
    {
        return LeftChild( Parent( i ) ) == i;
    }

    inline bool IsRightChild( I i ) const
    {
        return RightChild( Parent( i ) ) == i;
    }

    inline T& Element( I i ) { return m_Elements[i].m_Data; }
    inline T const& Element( I i ) const { return m_Elements[i].m_Data; }

    //-----------------------------------------------------------------------------
    // inserts a node into the tree
    //-----------------------------------------------------------------------------

    // Inserts a node into the tree, doesn't copy the data in.
    void FindInsertionPosition( T const& insert, I& parent, bool& leftchild )
    {
        /* find where node belongs */
        I current = m_Root;
        parent = InvalidIndex();
        leftchild = false;
        while ( current != InvalidIndex() )
        {
            parent = current;
            if ( m_LessFunc( insert, Element( current ) ) )
            {
                leftchild = true;
                current = LeftChild( current );
            }
            else
            {
                leftchild = false;
                current = RightChild( current );
            }
        }
    }

    I Insert( T const& insert )
    {
        // use copy constructor to copy it in
        I parent;
        bool leftchild;
        FindInsertionPosition( insert, parent, leftchild );
        I newNode = InsertAt( parent, leftchild );
        CopyConstruct( &Element( newNode ), insert );
        return newNode;
    }

    //-----------------------------------------------------------------------------
    // Insert a node into the tree
    //-----------------------------------------------------------------------------

    I NewNode()
    {
        I elem;

        // Nothing in the free list; add.
        if ( m_FirstFree == InvalidIndex() )
        {
            typename M::Iterator_t it =
                m_Elements.IsValidIterator( m_LastAlloc ) ?
                    m_Elements.Next( m_LastAlloc ) :
                    m_Elements.First();
            if ( !m_Elements.IsValidIterator( it ) )
            {
                m_Elements.Grow();

                it = m_Elements.IsValidIterator( m_LastAlloc ) ?
                         m_Elements.Next( m_LastAlloc ) :
                         m_Elements.First();

                if ( !m_Elements.IsValidIterator( it ) )
                {
                    throw new std::exception( "bad iterator" );
                }
            }
            m_LastAlloc = it;
            elem = m_Elements.GetIndex( m_LastAlloc );
        }
        else
        {
            elem = m_FirstFree;
            m_FirstFree = Links( m_FirstFree ).m_Right;
        }

#ifdef _DEBUG
        // reset links to invalid....
        Links_t& node = Links( elem );
        node.m_Left = node.m_Right = node.m_Parent = InvalidIndex();
#endif

        Construct( &Element( elem ) );
        ResetDbgInfo();

        return elem;
    }

    I InsertAt( I parent, bool leftchild )
    {
        I i = NewNode();
        LinkToParent( i, parent, leftchild );
        ++m_NumElements;

        return i;
    }

    void LinkToParent( I i, I parent, bool isLeft )
    {
        Links_t& elem = Links( i );
        elem.m_Parent = parent;
        elem.m_Left = elem.m_Right = InvalidIndex();
        elem.m_Tag = RED;

        /* insert node in tree */
        if ( parent != InvalidIndex() )
        {
            if ( isLeft )
                Links( parent ).m_Left = i;
            else
                Links( parent ).m_Right = i;
        }
        else
        {
            m_Root = i;
        }

        // InsertRebalance( i );
    }

    //-----------------------------------------------------------------------------
    // Checks if a node is valid and in the tree
    //-----------------------------------------------------------------------------
    inline bool IsValidIndex( I i ) const
    {
        if ( !m_Elements.IsIdxValid( i ) )
            return false;

        if ( m_Elements.IsIdxAfter( i, m_LastAlloc ) )
            return false;  // don't read values that have been allocated, but
                           // not constructed

        return LeftChild( i ) != i;
    }

    //-----------------------------------------------------------------------------
    // Removes all nodes from the tree
    //-----------------------------------------------------------------------------
    void RemoveAll()
    {
        // Have to do some convoluted stuff to invoke the destructor on all
        // valid elements for the multilist case (since we don't have all
        // elements connected to each other in a list).

        if ( m_LastAlloc == m_Elements.InvalidIterator() )
        {
            return;
        }

        for ( typename M::Iterator_t it = m_Elements.First();
              it != m_Elements.InvalidIterator(); it = m_Elements.Next( it ) )
        {
            I i = m_Elements.GetIndex( it );
            if ( IsValidIndex( i ) )  // skip elements in the free list
            {
                Destruct( &Element( i ) );
                SetRightChild( i, m_FirstFree );
                SetLeftChild( i, i );
                m_FirstFree = i;
            }

            if ( it == m_LastAlloc )
                break;  // don't destruct elements that haven't ever been
                        // constucted
        }

        // Clear everything else out
        m_Root = InvalidIndex();
        // Technically, this iterator could become invalid. It will not, because
        // it's always the same iterator. If we don't clear this here, the state
        // of this container will be invalid after we start inserting elements
        // again.
        m_LastAlloc = m_Elements.InvalidIterator();
        m_FirstFree = InvalidIndex();
        m_NumElements = 0;
    }

    //-----------------------------------------------------------------------------
    // Removes all nodes from the tree and purges memory
    //-----------------------------------------------------------------------------
    void Purge()
    {
        RemoveAll();
        m_Elements.Purge();
    }

    //-----------------------------------------------------------------------------
    // iteration
    //-----------------------------------------------------------------------------
    I FirstInorder() const
    {
        I i = m_Root;
        while ( LeftChild( i ) != InvalidIndex() )
            i = LeftChild( i );
        return i;
    }

    I NextInorder( I i ) const
    {
        if ( !IsValidIndex( i ) )
            return InvalidIndex();

        if ( RightChild( i ) != InvalidIndex() )
        {
            i = RightChild( i );
            while ( LeftChild( i ) != InvalidIndex() )
                i = LeftChild( i );
            return i;
        }

        I parent = Parent( i );
        while ( IsRightChild( i ) )
        {
            i = parent;
            if ( i == InvalidIndex() )
                break;
            parent = Parent( i );
        }
        return parent;
    }

    I PrevInorder( I i ) const
    {
        if ( !IsValidIndex( i ) )
            return InvalidIndex();

        if ( LeftChild( i ) != InvalidIndex() )
        {
            i = LeftChild( i );
            while ( RightChild( i ) != InvalidIndex() )
                i = RightChild( i );
            return i;
        }

        I parent = Parent( i );
        while ( IsLeftChild( i ) )
        {
            i = parent;
            if ( i == InvalidIndex() )
                break;
            parent = Parent( i );
        }
        return parent;
    }

    I LastInorder() const
    {
        I i = m_Root;
        while ( RightChild( i ) != InvalidIndex() )
            i = RightChild( i );
        return i;
    }
};