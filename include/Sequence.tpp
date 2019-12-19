
template <typename ElemType>
Sequence<ElemType>::Sequence() {}

template <typename ElemType>
Sequence<ElemType>::Sequence(std::size_t size) {
    _vec.reserve(size);
}

template <typename ElemType>
Sequence<ElemType>::Sequence(const Sequence& seq) {
    _vec.reserve(seq.size());

    for (std::size_t i=0; i<seq.size(); i++) {
        _vec.push_back(seq.at(i));
    }
}

template <typename ElemType>
Sequence<ElemType>::Sequence(Sequence&& seq) 
    : _vec(std::move(seq._vec)) {}

template <typename ElemType>
Sequence<ElemType>& Sequence<ElemType>::operator=(const Sequence& seq) {
    _vec.reserve(seq.size());

    for (size_t i=0; i<seq.size(); i++) {
        _vec.push_back(seq.at(i));
    }
}

template <typename ElemType>
Sequence<ElemType>& Sequence<ElemType>::operator=(Sequence&& seq) {
    _vec = std::move(seq._vec);
    return *this;
}

template <typename ElemType>
void Sequence<ElemType>::add(ElemType elem) {
    _vec.push_back(elem);
}

template <typename ElemType>
ElemType& Sequence<ElemType>::at(size_t i) {
    return _vec.at(i);
}

template <typename ElemType>
template <typename MapFunc>
void Sequence<ElemType>::map(MapFunc map_func) {
    for (auto& vec_elem : _vec) {
        vec_elem = map_func(vec_elem);
    }
}

template <typename ElemType>
template <typename OutputStream>
void Sequence<ElemType>::print(OutputStream& out_stream) {
    for (auto& vec_elem : _vec) {
        out_stream << vec_elem;
    }
}

template <typename ElemType>
template <typename TransFunc>
MonotonicSequence<ElemType>::MonotonicSequence(ElemType seed, std::size_t size, TransFunc trans_func) 
    : Sequence<ElemType>(size)
{
    this->add(seed); 
    for (std::size_t i=1; i<size; i++) {
        ElemType prev = this->at(i-1);
        this->add(trans_func(prev)); 
    }
}
