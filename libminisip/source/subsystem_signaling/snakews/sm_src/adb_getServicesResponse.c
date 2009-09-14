

        /**
         * adb_getServicesResponse.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_getServicesResponse.h"
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = getServicesResponse
                 * Namespace URI = http://esb.servicesmanager.services.hdviper.psnc.pl/
                 * Namespace Prefix = ns1
                 */
           


        struct adb_getServicesResponse
        {
            axutil_array_list_t* property_service;

                
                axis2_bool_t is_valid_service;


            
        };


       /************************* Private Function prototypes ********************************/
        
                 axis2_status_t AXIS2_CALL
                 adb_getServicesResponse_set_service_nil_at(
                        adb_getServicesResponse_t* _getServicesResponse, 
                        const axutil_env_t *env, int i);
                

                axis2_status_t AXIS2_CALL
                adb_getServicesResponse_set_service_nil(
                        adb_getServicesResponse_t* _getServicesResponse,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_getServicesResponse_t* AXIS2_CALL
        adb_getServicesResponse_create(
            const axutil_env_t *env)
        {
            adb_getServicesResponse_t *_getServicesResponse = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _getServicesResponse = (adb_getServicesResponse_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_getServicesResponse_t));

            if(NULL == _getServicesResponse)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_getServicesResponse, 0, sizeof(adb_getServicesResponse_t));

            _getServicesResponse->property_service  = NULL;
                  _getServicesResponse->is_valid_service  = AXIS2_FALSE;
            

            return _getServicesResponse;
        }

        axis2_status_t AXIS2_CALL
        adb_getServicesResponse_free (
                adb_getServicesResponse_t* _getServicesResponse,
                const axutil_env_t *env)
        {
            
                int i = 0;
                int count = 0;
                void *element = NULL;
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);

            adb_getServicesResponse_reset_service(_getServicesResponse, env);
            

            if(_getServicesResponse)
            {
                AXIS2_FREE(env->allocator, _getServicesResponse);
                _getServicesResponse = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_getServicesResponse_deserialize(
                adb_getServicesResponse_t* _getServicesResponse,
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
          
               int i = 0;
               axutil_array_list_t *arr_list = NULL;
            
               int sequence_broken = 0;
               axiom_node_t *tmp_node = NULL;
            
            axutil_qname_t *element_qname = NULL; 
            
               axiom_node_t *first_node = NULL;
               axis2_bool_t is_early_node_valid = AXIS2_TRUE;
               axiom_node_t *current_node = NULL;
               axiom_element_t *current_element = NULL;
            
            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for getServicesResponse : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, env);
                      
                    
                    /*
                     * building service array
                     */
                       arr_list = axutil_array_list_create(env, 10);
                   

                     
                     /*
                      * building service element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(env, "service", NULL, NULL);
                                  
                               
                               for (i = 0, sequence_broken = 0, current_node = first_node; !sequence_broken && current_node != NULL;) 
                                             
                               {
                                  if(axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                  {
                                     current_node =axiom_node_get_next_sibling(current_node, env);
                                     is_early_node_valid = AXIS2_FALSE;
                                     continue;
                                  }
                                  
                                  current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                  qname = axiom_element_get_qname(current_element, env, current_node);

                                  if (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("service", axiom_element_get_localname(current_element, env)))
                                  {
                                  
                                      is_early_node_valid = AXIS2_TRUE;
                                      
                                     
                                          element = (void*)adb_service_create(env);
                                          
                                          status =  adb_service_deserialize((adb_service_t*)element, env,
                                                                                 &current_node, &is_early_node_valid, AXIS2_FALSE);
                                          
                                          if(AXIS2_FAILURE ==  status)
                                          {
                                              AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building element service ");
                                          }
                                          else
                                          {
                                            axutil_array_list_add_at(arr_list, env, i, element);
                                          }
                                        
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for service ");
                                         if(element_qname)
                                         {
                                            axutil_qname_free(element_qname, env);
                                         }
                                         if(arr_list)
                                         {
                                            axutil_array_list_free(arr_list, env);
                                         }
                                         return AXIS2_FAILURE;
                                     }

                                     i ++;
                                    current_node = axiom_node_get_next_sibling(current_node, env);
                                  }
                                  else
                                  {
                                      is_early_node_valid = AXIS2_FALSE;
                                      sequence_broken = 1;
                                  }
                                  
                               }

                               
                                   if (i < 0)
                                   {
                                     /* found element out of order */
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "service (@minOccurs = '0') only have %d elements", i);
                                     if(element_qname)
                                     {
                                        axutil_qname_free(element_qname, env);
                                     }
                                     if(arr_list)
                                     {
                                        axutil_array_list_free(arr_list, env);
                                     }
                                     return AXIS2_FAILURE;
                                   }
                               

                               if(0 == axutil_array_list_size(arr_list,env))
                               {
                                    axutil_array_list_free(arr_list, env);
                               }
                               else
                               {
                                    status = adb_getServicesResponse_set_service(_getServicesResponse, env,
                                                                   arr_list);
                               }

                             
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 
          return status;
       }

          axis2_bool_t AXIS2_CALL
          adb_getServicesResponse_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_getServicesResponse_declare_parent_namespaces(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_getServicesResponse_serialize(
                adb_getServicesResponse_t* _getServicesResponse,
                const axutil_env_t *env, axiom_node_t *parent, axiom_element_t *parent_element, int parent_tag_closed, axutil_hash_t *namespaces, int *next_ns_index)
        {
            
            
             axis2_char_t *string_to_stream;
            
         
         axiom_node_t *current_node = NULL;
         int tag_closed = 0;

         
         
                axiom_namespace_t *ns1 = NULL;

                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                axis2_char_t *p_prefix = NULL;
                axis2_bool_t ns_already_defined;
            
               int i = 0;
               int count = 0;
               void *element = NULL;
             
                    axis2_char_t text_value_1[64];
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _getServicesResponse, NULL);
            
            
                    current_node = parent;
                    data_source = (axiom_data_source_t *)axiom_node_get_data_element(current_node, env);
                    if (!data_source)
                        return NULL;
                    stream = axiom_data_source_get_stream(data_source, env); /* assume parent is of type data source */
                    if (!stream)
                        return NULL;
                  
            if(!parent_tag_closed)
            {
            
              string_to_stream = ">"; 
              axutil_stream_write(stream, env, string_to_stream, axutil_strlen(string_to_stream));
              tag_closed = 1;
            
            }
            
                       p_prefix = NULL;
                      

                   if (!_getServicesResponse->is_valid_service)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("service"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("service")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing service array
                      */
                     if (_getServicesResponse->property_service != NULL)
                     {
                        

                            sprintf(start_input_str, "<%s%sservice",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%sservice>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = axutil_array_list_size(_getServicesResponse->property_service, env);
                         for(i = 0; i < count; i ++)
                         {
                            element = axutil_array_list_get(_getServicesResponse->property_service, env, i);

                            if(NULL == element) 
                            {
                                continue;
                            }
                    
                     
                     /*
                      * parsing service element
                      */

                    
                     
                            if(!adb_service_is_particle())
                            {
                                axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                            }
                            
                            adb_service_serialize((adb_service_t*)element, 
                                                                                 env, current_node, parent_element,
                                                                                 adb_service_is_particle() || AXIS2_FALSE, namespaces, next_ns_index);
                            
                            if(!adb_service_is_particle())
                            {
                                axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                            }
                            
                         }
                     }
                   
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 

            return parent;
        }


        

            /**
             * getter for service.
             */
            axutil_array_list_t* AXIS2_CALL
            adb_getServicesResponse_get_service(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getServicesResponse, NULL);
                  

                return _getServicesResponse->property_service;
             }

            /**
             * setter for service
             */
            axis2_status_t AXIS2_CALL
            adb_getServicesResponse_set_service(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env,
                    axutil_array_list_t*  arg_service)
             {
                
                 int size = 0;
                 int i = 0;
                 axis2_bool_t non_nil_exists = AXIS2_FALSE;
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);
                
                if(_getServicesResponse->is_valid_service &&
                        arg_service == _getServicesResponse->property_service)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                 size = axutil_array_list_size(arg_service, env);
                 
                 if (size < 0)
                 {
                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "service has less than minOccurs(0)");
                     return AXIS2_FAILURE;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != axutil_array_list_get(arg_service, env, i))
                     {
                         non_nil_exists = AXIS2_TRUE;
                         break;
                     }
                 }

                 adb_getServicesResponse_reset_service(_getServicesResponse, env);

                
                if(NULL == arg_service)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _getServicesResponse->property_service = arg_service;
                        if(non_nil_exists)
                        {
                            _getServicesResponse->is_valid_service = AXIS2_TRUE;
                        }
                        
                    
                return AXIS2_SUCCESS;
             }

            
            /**
             * Get ith element of service.
             */
            adb_service_t* AXIS2_CALL
            adb_getServicesResponse_get_service_at(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env, int i)
            {
                adb_service_t* ret_val;

                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getServicesResponse, NULL);
                  

                if(_getServicesResponse->property_service == NULL)
                {
                    return (adb_service_t*)0;
                }
                ret_val = (adb_service_t*)axutil_array_list_get(_getServicesResponse->property_service, env, i);
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of service.
             */
            axis2_status_t AXIS2_CALL
            adb_getServicesResponse_set_service_at(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env, int i,
                    adb_service_t* arg_service)
            {
                void *element = NULL;
                int size = 0;
                int j;
                int k;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);
                
                if( _getServicesResponse->is_valid_service &&
                    _getServicesResponse->property_service &&
                
                    arg_service == (adb_service_t*)axutil_array_list_get(_getServicesResponse->property_service, env, i))
                  
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                    if(NULL == arg_service)
                    {
                        if(_getServicesResponse->property_service != NULL)
                        {
                            size = axutil_array_list_size(_getServicesResponse->property_service, env);
                            for(j = 0, k = 0; j < size; j ++ )
                            {
                                if(i == j) continue; 
                                if(NULL != axutil_array_list_get(_getServicesResponse->property_service, env, i))
                                {
                                    k ++;
                                    non_nil_exists = AXIS2_TRUE;
                                    if(k >= 0)
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        non_nil_exists = AXIS2_TRUE;
                    }
                  
                if( k < 0)
                {
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of service is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }

                if(_getServicesResponse->property_service == NULL)
                {
                    _getServicesResponse->property_service = axutil_array_list_create(env, 10);
                }
                
                /* check whether there already exist an element */
                element = axutil_array_list_get(_getServicesResponse->property_service, env, i);
                if(NULL != element)
                {
                  
                  
                  
                        adb_service_free((adb_service_t*)element, env);
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _getServicesResponse->is_valid_service = AXIS2_FALSE;
                        axutil_array_list_set(_getServicesResponse->property_service , env, i, NULL);
                        
                        return AXIS2_SUCCESS;
                    }
                
                   axutil_array_list_set(_getServicesResponse->property_service , env, i, arg_service);
                  _getServicesResponse->is_valid_service = AXIS2_TRUE;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to service.
             */
            axis2_status_t AXIS2_CALL
            adb_getServicesResponse_add_service(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env,
                    adb_service_t* arg_service)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);

                
                    if(NULL == arg_service)
                    {
                      
                           return AXIS2_SUCCESS; 
                        
                    }
                  

                if(_getServicesResponse->property_service == NULL)
                {
                    _getServicesResponse->property_service = axutil_array_list_create(env, 10);
                }
                if(_getServicesResponse->property_service == NULL)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Failed in allocatting memory for service");
                    return AXIS2_FAILURE;
                    
                }
                
                   axutil_array_list_add(_getServicesResponse->property_service , env, arg_service);
                  _getServicesResponse->is_valid_service = AXIS2_TRUE;
                return AXIS2_SUCCESS;
             }

            /**
             * Get the size of the service array.
             */
            int AXIS2_CALL
            adb_getServicesResponse_sizeof_service(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env)
            {
                AXIS2_ENV_CHECK(env, -1);
                AXIS2_PARAM_CHECK(env->error, _getServicesResponse, -1);
                if(_getServicesResponse->property_service == NULL)
                {
                    return 0;
                }
                return axutil_array_list_size(_getServicesResponse->property_service, env);
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            axis2_status_t AXIS2_CALL
            adb_getServicesResponse_remove_service_at(
                    adb_getServicesResponse_t* _getServicesResponse,
                    const axutil_env_t *env, int i)
            {
                return adb_getServicesResponse_set_service_nil_at(_getServicesResponse, env, i);
            }

            

           /**
            * resetter for service
            */
           axis2_status_t AXIS2_CALL
           adb_getServicesResponse_reset_service(
                   adb_getServicesResponse_t* _getServicesResponse,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);
               

               
                  if (_getServicesResponse->property_service != NULL)
                  {
                      count = axutil_array_list_size(_getServicesResponse->property_service, env);
                      for(i = 0; i < count; i ++)
                      {
                         element = axutil_array_list_get(_getServicesResponse->property_service, env, i);
                
            
                
                if(element != NULL)
                {
                   
                   
                        adb_service_free((adb_service_t*)element, env);
                     element = NULL;
                }
            
                
                
                
                      }
                      axutil_array_list_free(_getServicesResponse->property_service, env);
                  }
                _getServicesResponse->is_valid_service = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether service is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_getServicesResponse_is_service_nil(
                   adb_getServicesResponse_t* _getServicesResponse,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_TRUE);
               
               return !_getServicesResponse->is_valid_service;
           }

           /**
            * Set service to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_getServicesResponse_set_service_nil(
                   adb_getServicesResponse_t* _getServicesResponse,
                   const axutil_env_t *env)
           {
               return adb_getServicesResponse_reset_service(_getServicesResponse, env);
           }

           
           /**
            * Check whether service is nill at i
            */
           axis2_bool_t AXIS2_CALL
           adb_getServicesResponse_is_service_nil_at(
                   adb_getServicesResponse_t* _getServicesResponse,
                   const axutil_env_t *env, int i)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_TRUE);
               
               return (_getServicesResponse->is_valid_service == AXIS2_FALSE ||
                        NULL == _getServicesResponse->property_service || 
                        NULL == axutil_array_list_get(_getServicesResponse->property_service, env, i));
           }

           /**
            * Set service to nill at i
            */
           axis2_status_t AXIS2_CALL
           adb_getServicesResponse_set_service_nil_at(
                   adb_getServicesResponse_t* _getServicesResponse,
                   const axutil_env_t *env, int i)
           {
                void *element = NULL;
                int size = 0;
                int j;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                int k = 0;

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getServicesResponse, AXIS2_FAILURE);

                if(_getServicesResponse->property_service == NULL ||
                            _getServicesResponse->is_valid_service == AXIS2_FALSE)
                {
                    
                    non_nil_exists = AXIS2_FALSE;
                }
                else
                {
                    size = axutil_array_list_size(_getServicesResponse->property_service, env);
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != axutil_array_list_get(_getServicesResponse->property_service, env, i))
                        {
                            k ++;
                            non_nil_exists = AXIS2_TRUE;
                            if( k >= 0)
                            {
                                break;
                            }
                        }
                    }
                }
                

                if( k < 0)
                {
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of service is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }
 
                if(_getServicesResponse->property_service == NULL)
                {
                    _getServicesResponse->is_valid_service = AXIS2_FALSE;
                    
                    return AXIS2_SUCCESS;
                }

                /* check whether there already exist an element */
                element = axutil_array_list_get(_getServicesResponse->property_service, env, i);
                if(NULL != element)
                {
                  
                  
                  
                        adb_service_free((adb_service_t*)element, env);
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _getServicesResponse->is_valid_service = AXIS2_FALSE;
                        axutil_array_list_set(_getServicesResponse->property_service , env, i, NULL);
                        return AXIS2_SUCCESS;
                    }
                

                
                axutil_array_list_set(_getServicesResponse->property_service , env, i, NULL);
                
                return AXIS2_SUCCESS;

           }

           

