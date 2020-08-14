
//

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::Fractal()
    : depth(-1), top_level(-1), root(nullptr), 
      type(Type::unbalanced), impl_type(ImplType::sequential) {}
 
template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>::Element::Element(const ElementInfo& elem_info)
    : info(elem_info), fractal(nullptr), parent(nullptr), children() {} 

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>& Fractal<ElemType,SeedType,Arity>::grow(int depth) 
{
    if (depth < 0) {
        std::cerr << "Fractal::grow():error: growth depth cannot be negative!";
        std::exit(EXIT_FAILURE);
    }

    this->top_level = depth+1;
    this->depth = depth;
    
    // precompute the geometric progression 
    // of level sizes 
    for (int i=0; i<=this->top_level; i++) {
        geo_progression.push_back(geo_sum(i));
    }
    
    this->elements_num = fractal_elements_num();
    this->leaves_num = depth_elements_num(depth);

    if (this->depth >= 0) {
        // root element of the fractal tree to be created
        ElementInfo info;
        info.level = this->top_level;
        info.depth = 0;
        info.child_id = 0;
        info.index = 0;

        if (type == Type::unbalanced) {
            root = grow_unbalanced(info);
        } else if (type == Type::balanced) {
            grow_balanced(info);
        } else {
            std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
            std::exit(EXIT_FAILURE);
        }
    }
    
    return *this;
}

template <typename ElemType, typename SeedType, int Arity>
Fractal<ElemType,SeedType,Arity>& Fractal<ElemType,SeedType,Arity>::grow(int depth, SeedType seed) 
{
    if (depth < 0) {
        std::cerr << "Fractal::grow():error: growth depth cannot be negative!";
        std::exit(EXIT_FAILURE);
    }

    this->top_level = depth+1;
    this->depth = depth;
    
    // precompute the geometric progression 
    // of level sizes 
    for (int i=0; i<=this->top_level; i++) {
        geo_progression.push_back(geo_sum(i));
    }
    
    this->elements_num = fractal_elements_num();
    this->leaves_num = depth_elements_num(depth);

    if (this->depth >= 0) {
        // root element of the fractal tree to be created
        ElementInfo info;
        info.level = this->top_level;
        info.depth = 0;
        info.child_id = 0;
        info.index = 0;

        if (type == Type::unbalanced) {
            root = grow_unbalanced(seed, info);
        } else if (type == Type::balanced) {
            grow_balanced(seed, info);
        } else {
            std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
            std::exit(EXIT_FAILURE);
        }
    }
    return *this;
}

template <typename ElemType, typename SeedType, int Arity>
std::unique_ptr<typename Fractal<ElemType,SeedType,Arity>::Element> Fractal<ElemType,SeedType,Arity>::grow_unbalanced(ElementInfo info) 
{
    if (info.level > 0) {
        
        //
        // CREATE THE ROOT ELEMENT OF THE FRACTAL SUBTREE
        //
        // allocate memory for the root of the fractal subtree 
        std::unique_ptr<Element> root_elem(new ElemType(info));
        // set fractal the element belongs to
        root_elem->set_fractal(this);
        // grow the root element
        root_elem->grow();
        
        // 
        // GROW CHILD SUBTREES
        // 

        // 
        if ( (info.level-1 > 0) && 
             (!root_elem->growth_stop_condition()) ) 
        {
            if (this->get_impl_type() == Fractal_t::ImplType::parallel) {
                if (info.depth < 1) {
                    // parallelize 
                    std::vector<std::unique_ptr<Element>> tmp(info.children_num);
                    int threads_count = (info.children_num <= 4) ? info.children_num : 4;

                    #pragma omp parallel num_threads(threads_count) shared(tmp)
                    {
                        #pragma omp for
                        for (int child_id = 0; child_id < Arity; child_id++) {
                            // root element of the subtree to be created
                            // element structural info
                            ElementInfo child_info;
                            child_info.level = info.level-1;
                            child_info.depth = info.depth+1;
                            child_info.child_id = child_id;
                            child_info.index = child_index(info.index,child_id+1);
                            
                            std::unique_ptr<Element>& child_elem = tmp[child_id];
                            child_elem = std::move(grow_unbalanced(child_info));
                            child_elem->set_parent_element(root_elem.get());
                        }
                    }

                    for (int child_id = 0; child_id < info.children_num; child_id++) {
                        root_elem->children.push_back(std::move(tmp[child_id]));
                    }

                } else {
                    for (int child_id = 0; child_id < Arity; child_id++) {
                        // root element of the subtree to be created
                        // element structural info
                        ElementInfo child_info;
                        child_info.level = info.level-1;
                        child_info.depth = info.depth+1;
                        child_info.child_id = child_id;
                        child_info.index = child_index(info.index,child_id+1);
                        
                        std::unique_ptr<Element> child_elem = std::move(grow_unbalanced(child_info));
                        child_elem->set_parent_element(root_elem.get());

                        root_elem->children.push_back(std::move(child_elem));
                    }
                }
            } else {
                for (int child_id = 0; child_id < Arity; child_id++) {
                    // root element of the subtree to be created
                    // element structural info
                    ElementInfo child_info;
                    child_info.level = info.level-1;
                    child_info.depth = info.depth+1;
                    child_info.child_id = child_id;
                    child_info.index = child_index(info.index,child_id+1);
                    
                    std::unique_ptr<Element> child_elem = std::move(grow_unbalanced(child_info));
                    child_elem->set_parent_element(root_elem.get());

                    root_elem->children.push_back(std::move(child_elem));
                }
            }
        }
        
        return root_elem;

    } else {
        return std::unique_ptr<Element>(nullptr);
    }
}

template <typename ElemType, typename SeedType, int Arity>
std::unique_ptr<typename Fractal<ElemType,SeedType,Arity>::Element> Fractal<ElemType,SeedType,Arity>::grow_unbalanced(SeedType seed, ElementInfo info) 
{
    if (info.level > 0) {
        
        //
        // CREATE THE ROOT ELEMENT OF THE FRACTAL SUBTREE
        //
        // allocate memory for the root of the fractal subtree 
        std::unique_ptr<Element> root_elem(new ElemType(info));
        // set fractal the element belongs to
        root_elem->set_fractal(this);
        // plant the seed
        root_elem->plant_seed(seed);
        // grow the root element
        root_elem->grow(seed);
        
        // 
        // GROW CHILD SUBTREES
        // 
        
        // 
        if ( (info.level-1 > 0) && 
             (!root_elem->growth_stop_condition()) ) 
        {
            if (this->get_impl_type() == Fractal_t::ImplType::parallel) {
                if (info.depth < 1) {
                    // parallelize 
                    std::vector<std::unique_ptr<Element>> tmp(info.children_num);
                    int threads_count = (info.children_num <= 4) ? info.children_num : 4;

                    #pragma omp parallel num_threads(threads_count) shared(tmp)
                    {
                        #pragma omp for
                        for (int child_id = 0; child_id < Arity; child_id++) {
                            // root element of the subtree to be created
                            // element structural info
                            ElementInfo child_info;
                            child_info.level = info.level-1;
                            child_info.depth = info.depth+1;
                            child_info.index = child_index(info.index,child_id+1);
                            child_info.child_id = child_id;

                            // seed to grow the child element
                            SeedType child_seed = root_elem->spawn_child_seed(child_id);
                        
                            std::unique_ptr<Element>& child_elem = tmp[child_id];
                            child_elem = std::move(grow_unbalanced(child_seed, child_info));
                            child_elem->set_parent_element(root_elem.get());
                        }
                    }

                    for (int child_id = 0; child_id < info.children_num; child_id++) {
                        root_elem->children.push_back(std::move(tmp[child_id]));
                    }

                } else {
                    for (int child_id = 0; child_id < Arity; child_id++) {
                        // root element of the subtree to be created
                        // element structural info
                        ElementInfo child_info;
                        child_info.level = info.level-1;
                        child_info.depth = info.depth+1;
                        child_info.child_id = child_id;
                        child_info.index = child_index(info.index,child_id+1);
                        
                        // seed to grow the child element
                        SeedType child_seed = root_elem->spawn_child_seed(child_id);
                        
                        std::unique_ptr<Element> child_elem = std::move(grow_unbalanced(child_seed, child_info));
                        child_elem->set_parent_element(root_elem.get());

                        root_elem->children.push_back(std::move(child_elem));
                    }
                }
            } else {
                for (int child_id = 0; child_id < Arity; child_id++) {
                    // root element of the subtree to be created
                    // element structural info
                    ElementInfo child_info;
                    child_info.level = info.level-1;
                    child_info.depth = info.depth+1;
                    child_info.child_id = child_id;
                    child_info.index = child_index(info.index,child_id+1);
                    
                    // seed to grow the child element
                    SeedType child_seed = root_elem->spawn_child_seed(child_id);
                    
                    std::unique_ptr<Element> child_elem = std::move(grow_unbalanced(child_seed, child_info));
                    child_elem->set_parent_element(root_elem.get());

                    root_elem->children.push_back(std::move(child_elem));
                }
            }
        }
        
        return root_elem;
    
    } else {
        return std::unique_ptr<Element>(nullptr);
    }
}

template <typename ElemType, typename SeedType, int Arity>
void Fractal<ElemType,SeedType,Arity>::grow_balanced(SeedType seed, ElementInfo info) 
{
    //
    // CREATE THE ROOT ELEMENT OF THE FRACTAL
    //
    // allocate memory for the root of the fractal
    std::unique_ptr<Element> root_elem(new ElemType(info));
    // set fractal the element belongs to
    root_elem->set_fractal(this);
    // plant the seed
    root_elem->plant_seed(seed);
    // grow the root element
    root_elem->grow(seed);
    
    if (get_impl_type() == ImplType::parallel) {
        elements.resize(elements_num);
        elements[0] = std::move(root_elem);
    } else {
        elements.push_back(std::move(root_elem));
    }

    for (int i = 0; i < elements_num-leaves_num; i++) {
        
        // for every fractal element with children
        // grow all its children
        
        if (get_impl_type() == ImplType::parallel) {

            int max_threads = omp_get_max_threads();
            int threads_count = (Arity <= max_threads) ? Arity : max_threads;

            #pragma omp parallel for num_threads(threads_count) shared(elements)
            for (int j = first_child(i); j <= last_child(i); j++) {
                ElementInfo child_info;
                child_info.level = index_to_level(j);
                child_info.depth = index_to_depth(j);
                child_info.child_id = j-first_child(i);
                child_info.index = j;

                // seed to grow the child element
                SeedType child_seed = elements[i]->spawn_child_seed(child_info.child_id);

                // allocate memory for the child element
                std::unique_ptr<Element> child_elem(new ElemType(child_info));
                // set fractal the element belongs to
                child_elem->set_fractal(this);
                // set parent element
                child_elem->set_parent_element(elements[i].get());
                // plant the seed
                child_elem->plant_seed(child_seed);
                // grow the child element
                child_elem->grow(child_seed);
                
                elements[j] = std::move(child_elem);
            }
        } else {
            for (int j = first_child(i); j <= last_child(i); j++) {
                ElementInfo child_info;
                child_info.level = index_to_level(j);
                child_info.depth = index_to_depth(j);
                child_info.child_id = j-first_child(i);
                child_info.index = j;

                // seed to grow the child element
                SeedType child_seed = elements[i]->spawn_child_seed(child_info.child_id);

                // allocate memory for the child element
                std::unique_ptr<Element> child_elem(new ElemType(child_info));
                // set fractal the element belongs to
                child_elem->set_fractal(this);
                // set parent element
                child_elem->set_parent_element(elements[i].get());
                // plant the seed
                child_elem->plant_seed(child_seed);
                // grow the child element
                child_elem->grow(child_seed);
                
                elements.push_back(std::move(child_elem));
            }
        }
    }

    return;
}

template <typename ElemType, typename SeedType, int Arity>
void Fractal<ElemType,SeedType,Arity>::grow_balanced(ElementInfo info) 
{
    //
    // CREATE THE ROOT ELEMENT OF THE FRACTAL
    //
    // allocate memory for the root of the fractal
    std::unique_ptr<Element> root_elem(new ElemType(info));
    // set fractal the element belongs to
    root_elem->set_fractal(this);
    // grow the root element
    root_elem->grow();
    
    if (get_impl_type() == ImplType::parallel) {
        elements.resize(elements_num);
        elements[0] = std::move(root_elem);
    } else {
        elements.push_back(std::move(root_elem));
    }

    for (int i = 0; i < elements_num-leaves_num; i++) {
        
        // for every fractal element with children
        // grow all its children
        
        if (get_impl_type() == ImplType::parallel) {

            int max_threads = omp_get_max_threads();
            int threads_count = (Arity <= max_threads) ? Arity : max_threads;

            #pragma omp parallel for num_threads(threads_count) shared(elements)
            for (int j = first_child(i); j <= last_child(i); j++) {
                ElementInfo child_info;
                child_info.level = index_to_level(j);
                child_info.depth = index_to_depth(j);
                child_info.child_id = j-first_child(i);
                child_info.index = j;

                // allocate memory for the child element
                std::unique_ptr<Element> child_elem(new ElemType(child_info));
                // set fractal the element belongs to
                child_elem->set_fractal(this);
                // set parent element
                child_elem->set_parent_element(elements[i].get());
                // grow the child element
                child_elem->grow();
                
                elements[j] = std::move(child_elem);
            }
        } else {
            for (int j = first_child(i); j <= last_child(i); j++) {
                ElementInfo child_info;
                child_info.level = index_to_level(j);
                child_info.depth = index_to_depth(j);
                child_info.child_id = j-first_child(i);
                child_info.index = j;

                // allocate memory for the child element
                std::unique_ptr<Element> child_elem(new ElemType(child_info));
                // set fractal the element belongs to
                child_elem->set_fractal(this);
                // set parent element
                child_elem->set_parent_element(elements[i].get());
                // grow the child element
                child_elem->grow();
                
                elements.push_back(std::move(child_elem));
            }
        }
    }

    return;
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::compute(ComputeFunction<ComputeType>& compute_func) {
    if (type == Type::unbalanced) {
        return this->template compute_unbalanced<ComputeType>(compute_func);
    } else if (type == Type::balanced) {
        return this->template compute_balanced<ComputeType>(compute_func);
    } else {
        std::cerr << "Fractal::grow():error: correct fractal type has not been specified!";
        std::exit(EXIT_FAILURE);
    }
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::compute_unbalanced(ComputeFunction<ComputeType>& compute_func) {
    if (root == nullptr) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }
    return root->template compute<ComputeType>(compute_func);
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::compute_balanced(ComputeFunction<ComputeType>& compute_func) {

    if (elements.empty()) {
        std::cerr << "Fractal::apply(): error: cannot apply the specified function to the NULL fractal root";
        std::exit(EXIT_FAILURE);
    }

    std::vector<ComputeType> computed_rets(elements_num);
    
    if (this->get_impl_type() == ImplType::parallel) {
        
        int max_threads = omp_get_max_threads();
        int threads_count;
        
        // precompute leaves
        
        threads_count = (leaves_num < max_threads) ? leaves_num : max_threads;

        #pragma omp parallel for num_threads(threads_count) shared(elements,computed_rets)
        for (int i = elements_num-1; i >= elements_num-leaves_num; i--) {
            std::vector<ComputeType> leaf_ret_vals;
            computed_rets[i] = compute_func(*(static_cast<ElemType*>(elements[i].get())), leaf_ret_vals);
        }

        for (int lvl = 2; lvl <= this->top_level; lvl++) {
            
            threads_count = (level_elements_num(lvl) < max_threads) ? level_elements_num(lvl) : max_threads;

            #pragma omp parallel for num_threads(threads_count) shared(elements,computed_rets,lvl)
            for (int i = level_start_index(lvl); i <= level_end_index(lvl); i++) {
                std::vector<ComputeType> ret_vals(Arity);
                // get child computation results 
                for (int j = first_child(i); j <= last_child(i); j++) {
                    ret_vals[j-first_child(i)] = computed_rets[j];
                }
                // perform computation for the element
                computed_rets[i] = compute_func(*(static_cast<ElemType*>(elements[i].get())), ret_vals);
            }
        }
    } else {
        // precompute leaves
        std::vector<ComputeType> leaf_ret_vals;
        for (int i = elements_num-1; i >= elements_num-leaves_num; i--) {
            computed_rets[i] = compute_func(*(static_cast<ElemType*>(elements[i].get())), leaf_ret_vals);
        }

        for (int i = elements_num-leaves_num-1; i >= 0; i--) {
            std::vector<ComputeType> ret_vals(Arity);
            // get child computation results 
            for (int j = first_child(i); j <= last_child(i); j++) {
                ret_vals[j-first_child(i)] = computed_rets[j];
            }
            // perform computation for the element
            computed_rets[i] = compute_func(*(static_cast<ElemType*>(elements[i].get())), ret_vals);
        }
    }
    
    return computed_rets[0];
}

template <typename ElemType, typename SeedType, int Arity>
template <typename ComputeType>
ComputeType Fractal<ElemType,SeedType,Arity>::Element::compute(ComputeFunction<ComputeType>& compute_func) {
    
    std::vector<ComputeType> ret_vals;
    
    if (this->info.level-1 > 0) 
    { 
        if (fractal->get_impl_type() == Fractal_t::ImplType::parallel) {
            if (info.depth < 2) {
                // parallelize 
                //std::vector<ComputeType> tmp(info.children_num);
                ComputeType tmp[Arity];
                //int threads_count = (info.children_num <= 4) ? info.children_num : 4;
                int threads_count = Arity;

                #pragma omp parallel num_threads(threads_count) shared(tmp)
                {
                    #pragma omp for schedule(static)
                    for (int i = 0; i < Arity; i++) {

                        //int tid = omp_get_thread_num();
                        //printf("Apply omp_thread=%d\n", tid);

                        tmp[i] = children[i]->template compute<ComputeType>(compute_func);
                    }
                }

                for (int i = 0; i < Arity; i++) {
                    ret_vals.push_back(tmp[i]);
                }
            } else {
                for (int i = 0; i < Arity; i++) {
                    ret_vals.push_back(children[i]->template compute<ComputeType>(compute_func));
                }
            }
        } else {
            for (int i = 0; i < Arity; i++) {
                ret_vals.push_back(children[i]->template compute<ComputeType>(compute_func));
            }
        }
    }
   
    return compute_func(*(static_cast<ElemType*>(this)), ret_vals);
}

// end
