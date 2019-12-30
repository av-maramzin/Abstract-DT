#ifndef COLLECTION_H
#define COLLECTION_H

namespace abstract {

template <typename ElemType>
class Collection {

    public:    
        
        Collection() {}
        virtual ~Collection() {}

        virtual void add(ElemType elem) = 0;
};

} // namespace abstract

#endif // #ifndef COLLECTION_H
