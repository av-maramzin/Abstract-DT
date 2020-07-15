#ifndef ABSTRACT_REDUCE_H
#define ABSTRACT_REDUCE_H

#include <iostream>
#include <vector>
#include <memory>

namespace abstract {

template <typename ElemType, typename SeedType, typename InjectType>
class Reduce
{
    public:

        using Element_t = ElemType;
        using Seed_t = SeedType;
        using Inject_t = InjectType;

        enum class ImplType {
            sequential = 0,
            parallel
        };

        // ElementInfo class 
        //
        // builds the location information of an element 
        // withing the fold into the element
        //
        struct ElementInfo;

        // Element class
        // 
        // Users of the Fold class template must specify their own 
        // type of elements to be folded by deriving them from below
        // Element class. The latter serves as the backbone of the 
        // fold, knows its depth and provides a customization interface
        //
        class Element;

        // ComputeFunction class
        // 
        // Users perform various kinds of reductions by providing 
        // custom reduce functions. The latter are supposed to be 
        // function objects derived from a below base class, 
        // which sets the interface to override
        //
        template <typename ComputeType>
        class ComputeFunction;

        Reduce();
        ~Reduce();
       
        // global interface to the Reduce class
        Reduce<ElemType,SeedType,InjectType>& grow(size_t width);
        Reduce<ElemType,SeedType,InjectType>& grow(size_t width, SeedType seed);
        //Reduce<ElemType,SeedType>& shrink(size_t width);
       
        // 
        // inject()
        //
        // injects the data into all Reduce elements. Injected data 
        // can be further used inside compute method()
        //
        Reduce<ElemType,SeedType,InjectType>& inject(const InjectType data);

        //
        // main compute() interface
        //
        // Takes custom compute function object as a parameter
        //
        template<typename ComputeType>
        ComputeType compute(Reduce<ElemType,SeedType,InjectType>::ComputeFunction<ComputeType>& func);

        void set_impl_type(ImplType t) { impl_type = t; }
        ImplType get_impl_type() const { return impl_type; }

    private:
        
        ImplType impl_type;
        int width;
        std::vector<std::unique_ptr<Element>> elements;
};

template <typename ElemType, typename SeedType, typename InjectType> 
struct Reduce<ElemType,SeedType,InjectType>::ElementInfo { 
    
    public:
        
        ElementInfo() 
            : index(-1) {} 

        ElementInfo(const ElementInfo& ref) 
            : index(ref.index) {} 

        int index;
};

template <typename ElemType, typename SeedType, typename InjectType> 
class Reduce<ElemType,SeedType,InjectType>::Element { 

    friend class Reduce<ElemType,SeedType,InjectType>;

    public:

        // customization interface 
        Element(const ElementInfo& info);
        virtual ~Element();

        virtual void grow() {}
        virtual void grow(SeedType seed) {}

        virtual void inject(const InjectType data) {
            injected_data = data;   
        }

        InjectType get_injected_data() { return injected_data; }

        void plant_seed(SeedType s) { seed = s; }
        const SeedType extract_seed() const { return seed; }

        const ElementInfo& element_info() const { return info; }

    private:
        // specify the reduce object this element belongs to
        void set_reduce(Reduce<ElemType,SeedType,InjectType>* r) { reduce = r; }

    private:
        // the seed that has been used to grow the element
        InjectType injected_data;
        // the seed that has been used to grow the element
        SeedType seed;
        // structural information
        ElementInfo info;
        // reduce the element belongs to
        Reduce<ElemType,SeedType,InjectType>* reduce;
};

template <typename ElemType, typename SeedType, typename InjectType>
template <typename ComputeType>
class Reduce<ElemType,SeedType,InjectType>::ComputeFunction {
    
    public:
        
        using Compute_t = ComputeType;
       
        //
        // Users of the Reduce class are supposed to override two
        // customization function call operators: one for instructions
        // on how to reduce the data from a single Reduce element, one
        // for instructions on how to combine all reduced values into
        // a final return value
        //

        //
        // Funtion to specify how to reduce the data from a single element
        //
        virtual ComputeType operator()(ElemType& element) {
            // stub version does not do any changes to the element
            // and just passes out its default value
            ComputeType ret;
            return ret;
        }

        //
        // Function to specify how to combine all reduced values
        //
        virtual ComputeType operator()(std::vector<ComputeType>& rets) {
            ComputeType ret;
            for (auto it = rets.begin(); it != rets.end(); it++) {
                ret += *it;                    
            }
            return ret;
        }
};

#include "Reduce.tpp"

}

#endif // #ifndef ABSTRACT_REDUCE_H
