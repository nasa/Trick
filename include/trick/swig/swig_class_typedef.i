
/*
  This is the 1D array support code for SWIG

  TYPE is the actual type of the variable (int, double, units::values::m)
  NAME is the type with ':' substituted with '_' (int, double, units__values__m)

 */

%define %trick_swig_class_typemap(TYPE, NAME)

%typemap(memberin) TYPE [ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%typemap(out) TYPE [ANY] {

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 1 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[0].start  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 1 ;
    t->ref.ref_type  = REF_ADDRESS ;

    t->ref.attr->units = strdup("1") ;

    //cout << "returning a structured swig_ref" << std::endl ;
    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(in) TYPE * , TYPE ** {
    // TYPE *
    void * temp_ptr ;

    SwigPyObject *sobj = SWIG_Python_GetSwigThis($input) ;
    // isDisown is set to SWIG_POINTER_DISOWN by default
    int isDisown = SWIG_POINTER_DISOWN ;
    if (sobj) {
        // isDisown is re-set based on whether the Python processor owns the input object 
        isDisown = !sobj->own ;
    }
    
    if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,$1_descriptor, isDisown)) ) {
        $1 = reinterpret_cast< $1_ltype >(temp_ptr) ;
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,SWIG_TypeQuery("_p_swig_ref"), 0)) ) {
        // Array to pointer assignment
        swig_ref * temp_swig_ref = reinterpret_cast< swig_ref * >(temp_ptr);
        if ( temp_swig_ref != NULL ) {
            $1 = reinterpret_cast< $1_ltype >(temp_swig_ref->ref.address) ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,SWIG_TypeQuery("_p_REF2"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        REF2 * temp_ref = reinterpret_cast< REF2 * >(temp_ptr) ;
        if ( temp_ref != NULL ){
            $1 = reinterpret_cast< $1_ltype >(temp_ref->address) ;
        }
    } else if ( SWIG_IsOK(SWIG_ConvertPtr($input, &temp_ptr,SWIG_TypeQuery("_p_void"), 0)) ) {
        // We have an address coming in, we don't have to do any translation
        $1 = reinterpret_cast< $1_ltype >(temp_ptr) ;
    }
}

/*
   Note: There is no "%typemap(out) TYPE *" defined because having one conflicts with
   what SWIG wants to do when allocating new classes. The extend TYPE __getitem__ at
   the bottom of this file provides us indexing to single dimension class variables.
 */

%typemap(out) TYPE * [ANY] {

    //TYPE * [ANY] OUT

    $1_basetype dummy ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[1].size  =  0 ;
    t->ref.attr->units = strdup("1") ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);

}

%typemap(out) TYPE ** {
    // TYPE ** out
    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = get_truncated_size((char *)t->ref.address) ;
    t->ref.attr->index[1].size  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    t->ref.attr->units = strdup("1") ;

    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(memberin) TYPE [ANY][ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%typemap(out) TYPE [ANY][ANY] {

    std::string temp_name ;

    swig_ref * t = new swig_ref ;

    t->ref.address = (void *)$1;
    t->ref.units = NULL ;

    t->ref.attr = new ATTRIBUTES() ;

    t->ref.attr->size = sizeof($1_basetype) ;
    t->ref.attr->type = TRICK_STRUCTURED ;

    t->ref.attr->type_name  = strdup("$1_basetype") ;
    t->ref.attr->units = NULL ;
    t->ref.attr->attr  = NULL ;
    t->ref.attr->io  = TRICK_VAR_OUTPUT | TRICK_VAR_INPUT | TRICK_CHKPNT_OUTPUT | TRICK_CHKPNT_INPUT ;

    t->ref.attr->num_index  = 2 ;
    t->ref.attr->index[0].size  = $1_dim0 ;
    t->ref.attr->index[0].start  = 0 ;
    t->ref.attr->index[1].size  = $1_dim1 ;
    t->ref.attr->index[1].start  = 0 ;

    t->ref.create_add_path  = 0 ;
    t->ref.num_index  = 0 ;
    t->ref.num_index_left  = 2 ;
    t->ref.ref_type  = REF_ADDRESS ;

    t->ref.attr->units = strdup("1") ;

    //cout << "returning a structured swig_ref" << std::endl ;
    $result = SWIG_NewPointerObj(SWIG_as_voidptr(t), SWIG_TypeQuery("_p_swig_ref"), SWIG_POINTER_OWN);
}

%typemap(memberin) TYPE [ANY][ANY][ANY] {
    std::cout << "Cannot assign directly to $1_type\n" << std::endl ;
}

%newobject TYPE::__dynamic_cast ;

%extend TYPE {
    PyObject * __getitem__(int ii) {

        PyObject *resultobj ;
        int array_size = (int) get_size((char *)self) ;

        if ( ii < 0 ) {
            ii += array_size ;
        }

        if ( (array_size > 0) and (ii > array_size) ) {
            return NULL ;
        }

        //std::cout << "HERE in class __getitem__!!!" << std::endl ;
        resultobj = SWIG_NewPointerObj(SWIG_as_voidptr(&self[ii]), SWIG_TypeQuery("TYPE *"), 0);
        return(resultobj) ;
    }

    PyObject * __len__() {
        // get_size only works if "self" was an allocated pointer
        return PyInt_FromLong(get_size((char *)self)) ;
    }
}

/* The following 2 feature("shadow") blocks override the __init__ constructors in the python
   proxy class.  These constructors notify the memory manager that new memory has been
   allocated if a named argument named TMMName is included.  The value of TMMName
   is used as the memory manager name handle to the memory.

   The difference in the 2 blocks is that the first one handles constructor(s) with
   arguments, the second one handles classes that have only a single default constructor.
 */

%feature("shadow") TYPE::TYPE %{
    def __init__(self, *args, **kwargs):
        import _sim_services
        using_alloc = False
        try:
          this = TYPE.alloc(*args)
          using_alloc = True
        except:
          this = $action(*args)
        try:
            this.append(this)
        except:
            self.this = this
        if 'TMMName' in kwargs:
            if not using_alloc:
                this.own(0)
                self.this.own(0)
            #This should still be valid
            isThisInMM = _sim_services.get_alloc_info_at(this)
            if isThisInMM:
                _sim_services.set_alloc_name_at(this, kwargs['TMMName'])
            else:
                _sim_services.TMM_declare_ext_var(this, _sim_services.TRICK_STRUCTURED, "TYPE", 0, kwargs['TMMName'], 0, None)
            if not self.using_alloc:
                alloc_info = _sim_services.get_alloc_info_at(this)
                alloc_info.stcl = _sim_services.TRICK_LOCAL
                alloc_info.alloc_type = _sim_services.TRICK_ALLOC_NEW
%}

%feature("shadow") TYPE::TYPE() %{
    def __init__(self, **kwargs):
        import _sim_services
        this = $action()
        try: self.this.append(this)
        except: self.this = this
        if 'TMMName' in kwargs:
            self.this.own(0)
            isThisInMM = _sim_services.get_alloc_info_at(this)
            if isThisInMM:
                _sim_services.set_alloc_name_at(this, kwargs['TMMName'])
            else:
                _sim_services.TMM_declare_ext_var(this, _sim_services.TRICK_STRUCTURED, "TYPE", 0, kwargs['TMMName'], 0, None)
            alloc_info = _sim_services.get_alloc_info_at(this)
            alloc_info.stcl = _sim_services.TRICK_LOCAL
            alloc_info.alloc_type = _sim_services.TRICK_ALLOC_NEW
%}



%enddef

