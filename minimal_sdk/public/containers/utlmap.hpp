#pragma once

#include "containers/utlrbtree.hpp"

template <typename K, typename T, typename I = unsigned short>
class CUtlMap
{
public:
    using KeyType_t = K;
    using ElemType_t = T;
    using IndexType_t = I;

    struct Node_t
    {
        Node_t() {}
        Node_t( const Node_t& from ) : key( from.key ), elem( from.elem ) {}

        KeyType_t key;
        ElemType_t elem;
    };

    // Less func typedef
    // Returns true if the first parameter is "less" than the second
    typedef bool ( *LessFunc_t )( const KeyType_t&, const KeyType_t& );

    class CKeyLess
    {
    public:
        CKeyLess( LessFunc_t lessFunc ) : m_LessFunc( lessFunc ) {}

        bool operator!() const { return !m_LessFunc; }

        bool operator()( const Node_t& left, const Node_t& right ) const
        {
            return m_LessFunc( left.key, right.key );
        }

        LessFunc_t m_LessFunc;
    };

    void SetLessFunc( LessFunc_t func )
    {
        m_Tree.SetLessFunc( CKeyLess( func ) );
    }

    // constructor, destructor
    // Left at growSize = 0, the memory will first allocate 1 element and double
    // in size at each increment. LessFunc_t is required, but may be set after
    // the constructor using SetLessFunc() below
    CUtlMap( int growSize = 0, int initSize = 0, LessFunc_t lessfunc = 0 )
        : m_Tree( growSize, initSize, CKeyLess( lessfunc ) )
    {
    }

    CUtlMap( LessFunc_t lessfunc ) : m_Tree( CKeyLess( lessfunc ) ) {}

    // Iteration
    IndexType_t FirstInorder() const { return m_Tree.FirstInorder(); }
    IndexType_t NextInorder( IndexType_t i ) const
    {
        return m_Tree.NextInorder( i );
    }
    IndexType_t PrevInorder( IndexType_t i ) const
    {
        return m_Tree.PrevInorder( i );
    }
    IndexType_t LastInorder() const { return m_Tree.LastInorder(); }

    IndexType_t Insert( const KeyType_t& key, const ElemType_t& insert )
    {
        Node_t node;
        node.key = key;
        node.elem = insert;
        return m_Tree.Insert( node );
    }

    // gets particular elements
    ElemType_t& Element( IndexType_t i ) { return m_Tree.Element( i ).elem; }
    const ElemType_t& Element( IndexType_t i ) const
    {
        return m_Tree.Element( i ).elem;
    }
    ElemType_t& operator[]( IndexType_t i ) { return m_Tree.Element( i ).elem; }
    const ElemType_t& operator[]( IndexType_t i ) const
    {
        return m_Tree.Element( i ).elem;
    }
    KeyType_t& Key( IndexType_t i ) { return m_Tree.Element( i ).key; }
    const KeyType_t& Key( IndexType_t i ) const
    {
        return m_Tree.Element( i ).key;
    }

    void RemoveAll() { m_Tree.RemoveAll(); }
    void Purge() { m_Tree.Purge(); }

    // Invalid index
    static IndexType_t InvalidIndex() { return CTree::InvalidIndex(); }

    using CTree = CUtlRBTree<Node_t, I, CKeyLess>;
    CTree m_Tree;
};
