#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <cstdlib>

#include <vector>
#include <utility>

#include <Collection.h>

namespace abstract {

template <typename ElemType>
class Sequence : public Collection<ElemType> {

    public:    
        
        Sequence();
        Sequence(std::size_t size);
        Sequence(const Sequence& seq);
        Sequence(Sequence&& seq);

        Sequence& operator=(const Sequence& seq);
        Sequence& operator=(Sequence&& seq);

        // primitive operations
        std::size_t size() const { return _vec.size(); }

        void add(ElemType elem);
        ElemType& at(std::size_t i);

        template<typename MapFunc>
        void map(MapFunc map_func);

        template<typename OutputStream>
        void print(OutputStream& out_stream);

    protected:
        std::vector<ElemType> _vec;
};

template <typename ElemType>
class MonotonicSequence : public Sequence<ElemType> {

    public:
        
        template <typename TransFunc>
        MonotonicSequence(ElemType seed, std::size_t size, TransFunc trans_func);
};

#include "Sequence.tpp"

}

#endif // #ifndef SEQUENCE_H
