

        /**
         * adb_getEventsResponse0.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_getEventsResponse0.h"
        
               /*
                * implmentation of the getEventsResponse|http://esb.callbackservice.services.hdviper.psnc.pl/ element
                */
           


        struct adb_getEventsResponse0
        {
            
                axutil_qname_t* qname;
            adb_getEventsResponse_t* property_getEventsResponse;

                
                axis2_bool_t is_valid_getEventsResponse;


            
        };


       /************************* Private Function prototypes ********************************/
        

                axis2_status_t AXIS2_CALL
                adb_getEventsResponse0_set_getEventsResponse_nil(
                        adb_getEventsResponse0_t* _getEventsResponse0,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_getEventsResponse0_t* AXIS2_CALL
        adb_getEventsResponse0_create(
            const axutil_env_t *env)
        {
            adb_getEventsResponse0_t *_getEventsResponse0 = NULL;
            
                axutil_qname_t* qname = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _getEventsResponse0 = (adb_getEventsResponse0_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_getEventsResponse0_t));

            if(NULL == _getEventsResponse0)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_getEventsResponse0, 0, sizeof(adb_getEventsResponse0_t));

            _getEventsResponse0->property_getEventsResponse  = NULL;
                  _getEventsResponse0->is_valid_getEventsResponse  = AXIS2_FALSE;
            
                  qname =  axutil_qname_create (env,
                        "getEventsResponse",
                        "http://esb.callbackservice.services.hdviper.psnc.pl/",
                        NULL);
                _getEventsResponse0->qname = qname;
            

            return _getEventsResponse0;
        }

        axis2_status_t AXIS2_CALL
        adb_getEventsResponse0_free (
                adb_getEventsResponse0_t* _getEventsResponse0,
                const axutil_env_t *env)
        {
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, AXIS2_FAILURE);

            adb_getEventsResponse0_reset_getEventsResponse(_getEventsResponse0, env);
            
              if(_getEventsResponse0->qname)
              {
                  axutil_qname_free (_getEventsResponse0->qname, env);
                  _getEventsResponse0->qname = NULL;
              }
            

            if(_getEventsResponse0)
            {
                AXIS2_FREE(env->allocator, _getEventsResponse0);
                _getEventsResponse0 = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_getEventsResponse0_deserialize(
                adb_getEventsResponse0_t* _getEventsResponse0,
                const axutil_env_t *env,
                axiom_node_t **dp_parent,
                axis2_bool_t *dp_is_early_node_valid,
                axis2_bool_t dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          axis2_status_t status = AXIS2_SUCCESS;
          
              void *element = NULL;
           
             axis2_char_t* text_value = NULL;
             axutil_qname_t *qname = NULL;
          
            axutil_qname_t *element_qname = NULL; 
            
               axiom_node_t *first_node = NULL;
               axis2_bool_t is_early_node_valid = AXIS2_TRUE;
               axiom_node_t *current_node = NULL;
               axiom_element_t *current_element = NULL;
            
            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for getEventsResponse : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              

                    current_element = (axiom_element_t *)axiom_node_get_data_element(parent, env);
                    qname = axiom_element_get_qname(current_element, env, parent);
                    if (axutil_qname_equals(qname, env, _getEventsResponse0-> qname))
                    {
                        
                          first_node = parent;
                          
                    }
                    else
                    {
                        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                              "Failed in building adb object for getEventsResponse : "
                              "Expected %s but returned %s",
                              axutil_qname_to_string(_getEventsResponse0-> qname, env),
                              axutil_qname_to_string(qname, env));
                        
                        return AXIS2_FAILURE;
                    }
                    

                     
                     /*
                      * building getEventsResponse element
                      */
                     
                     
                     
                                   current_node = first_node;
                                   is_early_node_valid = AXIS2_FALSE;
                                   
                                   
                                    while(current_node && axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                    {
                                        current_node = axiom_node_get_next_sibling(current_node, env);
                                    }
                                    if(current_node != NULL)
                                    {
                                        current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                        qname = axiom_element_get_qname(current_element, env, current_node);
                                    }
                                   
                                 element_qname = axutil_qname_create(env, "getEventsResponse", "http://esb.callbackservice.services.hdviper.psnc.pl/", NULL);
                                 

                           if (adb_getEventsResponse_is_particle() ||  
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname)))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      element = (void*)adb_getEventsResponse_create(env);

                                      status =  adb_getEventsResponse_deserialize((adb_getEventsResponse_t*)element,
                                                                            env, &current_node, &is_early_node_valid, AXIS2_FALSE);
                                      if(AXIS2_FAILURE == status)
                                      {
                                          AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building adb object for element getEventsResponse");
                                      }
                                      else
                                      {
                                          status = adb_getEventsResponse0_set_getEventsResponse(_getEventsResponse0, env,
                                                                   (adb_getEventsResponse_t*)element);
                                      }
                                    
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for getEventsResponse ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, env);
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                              else if(!dont_care_minoccurs)
                              {
                                  if(element_qname)
                                  {
                                      axutil_qname_free(element_qname, env);
                                  }
                                  /* this is not a nillable element*/
                                  AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "non nillable or minOuccrs != 0 element getEventsResponse missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 
          return status;
       }

          axis2_bool_t AXIS2_CALL
          adb_getEventsResponse0_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_getEventsResponse0_declare_parent_namespaces(
                    adb_getEventsResponse0_t* _getEventsResponse0,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_getEventsResponse0_serialize(
                adb_getEventsResponse0_t* _getEventsResponse0,
                const axutil_env_t *env, axiom_node_t *parent, axiom_element_t *parent_element, int parent_tag_closed, axutil_hash_t *namespaces, int *next_ns_index)
        {
            
            
         
         axiom_node_t *current_node = NULL;
         int tag_closed = 0;

         
         
                axiom_namespace_t *ns1 = NULL;

                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                axis2_char_t *p_prefix = NULL;
                axis2_bool_t ns_already_defined;
            
                    axis2_char_t text_value_1[64];
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

             
                int next_ns_index_value = 0;
            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, NULL);
            
             
                    namespaces = axutil_hash_make(env);
                    next_ns_index = &next_ns_index_value;
                     
                           ns1 = axiom_namespace_create (env,
                                             "http://esb.callbackservice.services.hdviper.psnc.pl/",
                                             "n"); 
                           axutil_hash_set(namespaces, "http://esb.callbackservice.services.hdviper.psnc.pl/", AXIS2_HASH_KEY_STRING, axutil_strdup(env, "n"));
                       
                     
                    parent_element = axiom_element_create (env, NULL, "getEventsResponse", ns1 , &parent);
                    
                    
                    axiom_element_set_namespace(parent_element, env, ns1, parent);


            
                    data_source = axiom_data_source_create(env, parent, &current_node);
                    stream = axiom_data_source_get_stream(data_source, env);
                  
                       if(!(p_prefix = (axis2_char_t*)axutil_hash_get(namespaces, "http://esb.callbackservice.services.hdviper.psnc.pl/", AXIS2_HASH_KEY_STRING)))
                       {
                           p_prefix = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof (axis2_char_t) * ADB_DEFAULT_NAMESPACE_PREFIX_LIMIT);
                           sprintf(p_prefix, "n%d", (*next_ns_index)++);
                           axutil_hash_set(namespaces, "http://esb.callbackservice.services.hdviper.psnc.pl/", AXIS2_HASH_KEY_STRING, p_prefix);
                           
                           axiom_element_declare_namespace_assume_param_ownership(parent_element, env, axiom_namespace_create (env,
                                            "http://esb.callbackservice.services.hdviper.psnc.pl/",
                                            p_prefix));
                       }
                      

                   if (!_getEventsResponse0->is_valid_getEventsResponse)
                   {
                      
                            
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Nil value found in non-nillable property getEventsResponse");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("getEventsResponse"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("getEventsResponse")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing getEventsResponse element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sgetEventsResponse",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sgetEventsResponse>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                            adb_getEventsResponse_serialize(_getEventsResponse0->property_getEventsResponse, 
                                                                                 env, current_node, parent_element,
                                                                                 adb_getEventsResponse_is_particle() || AXIS2_TRUE, namespaces, next_ns_index);
                            
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                   if(namespaces)
                   {
                       axutil_hash_index_t *hi;
                       void *val;
                       for (hi = axutil_hash_first(namespaces, env); hi; hi = axutil_hash_next(env, hi)) 
                       {
                           axutil_hash_this(hi, NULL, NULL, &val);
                           AXIS2_FREE(env->allocator, val);
                       }
                       axutil_hash_free(namespaces, env);
                   }
                

            return parent;
        }


        

            /**
             * getter for getEventsResponse.
             */
            adb_getEventsResponse_t* AXIS2_CALL
            adb_getEventsResponse0_get_getEventsResponse(
                    adb_getEventsResponse0_t* _getEventsResponse0,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, NULL);
                  

                return _getEventsResponse0->property_getEventsResponse;
             }

            /**
             * setter for getEventsResponse
             */
            axis2_status_t AXIS2_CALL
            adb_getEventsResponse0_set_getEventsResponse(
                    adb_getEventsResponse0_t* _getEventsResponse0,
                    const axutil_env_t *env,
                    adb_getEventsResponse_t*  arg_getEventsResponse)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, AXIS2_FAILURE);
                
                if(_getEventsResponse0->is_valid_getEventsResponse &&
                        arg_getEventsResponse == _getEventsResponse0->property_getEventsResponse)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                  if(NULL == arg_getEventsResponse)
                  {
                      AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "getEventsResponse is being set to NULL, but it is not a nullable element");
                      return AXIS2_FAILURE;
                  }
                adb_getEventsResponse0_reset_getEventsResponse(_getEventsResponse0, env);

                
                if(NULL == arg_getEventsResponse)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _getEventsResponse0->property_getEventsResponse = arg_getEventsResponse;
                        _getEventsResponse0->is_valid_getEventsResponse = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for getEventsResponse
            */
           axis2_status_t AXIS2_CALL
           adb_getEventsResponse0_reset_getEventsResponse(
                   adb_getEventsResponse0_t* _getEventsResponse0,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, AXIS2_FAILURE);
               

               
            
                
                if(_getEventsResponse0->property_getEventsResponse != NULL)
                {
                   
                   
                        adb_getEventsResponse_free(_getEventsResponse0->property_getEventsResponse, env);
                     _getEventsResponse0->property_getEventsResponse = NULL;
                }
            
                
                
                _getEventsResponse0->is_valid_getEventsResponse = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether getEventsResponse is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_getEventsResponse0_is_getEventsResponse_nil(
                   adb_getEventsResponse0_t* _getEventsResponse0,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getEventsResponse0, AXIS2_TRUE);
               
               return !_getEventsResponse0->is_valid_getEventsResponse;
           }

           /**
            * Set getEventsResponse to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_getEventsResponse0_set_getEventsResponse_nil(
                   adb_getEventsResponse0_t* _getEventsResponse0,
                   const axutil_env_t *env)
           {
               return adb_getEventsResponse0_reset_getEventsResponse(_getEventsResponse0, env);
           }

           

