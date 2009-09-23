

        /**
         * adb_getReservationStatusesResponse.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_getReservationStatusesResponse.h"
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = getReservationStatusesResponse
                 * Namespace URI = http://esb.bod.services.hdviper.i2cat.net/
                 * Namespace Prefix = ns1
                 */
           


        struct adb_getReservationStatusesResponse
        {
            axutil_array_list_t* property_reservationStatuses;

                
                axis2_bool_t is_valid_reservationStatuses;


            
        };


       /************************* Private Function prototypes ********************************/
        
                 axis2_status_t AXIS2_CALL
                 adb_getReservationStatusesResponse_set_reservationStatuses_nil_at(
                        adb_getReservationStatusesResponse_t* _getReservationStatusesResponse, 
                        const axutil_env_t *env, int i);
                

                axis2_status_t AXIS2_CALL
                adb_getReservationStatusesResponse_set_reservationStatuses_nil(
                        adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_getReservationStatusesResponse_t* AXIS2_CALL
        adb_getReservationStatusesResponse_create(
            const axutil_env_t *env)
        {
            adb_getReservationStatusesResponse_t *_getReservationStatusesResponse = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _getReservationStatusesResponse = (adb_getReservationStatusesResponse_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_getReservationStatusesResponse_t));

            if(NULL == _getReservationStatusesResponse)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_getReservationStatusesResponse, 0, sizeof(adb_getReservationStatusesResponse_t));

            _getReservationStatusesResponse->property_reservationStatuses  = NULL;
                  _getReservationStatusesResponse->is_valid_reservationStatuses  = AXIS2_FALSE;
            

            return _getReservationStatusesResponse;
        }

        axis2_status_t AXIS2_CALL
        adb_getReservationStatusesResponse_free (
                adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                const axutil_env_t *env)
        {
            
                int i = 0;
                int count = 0;
                void *element = NULL;
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);

            adb_getReservationStatusesResponse_reset_reservationStatuses(_getReservationStatusesResponse, env);
            

            if(_getReservationStatusesResponse)
            {
                AXIS2_FREE(env->allocator, _getReservationStatusesResponse);
                _getReservationStatusesResponse = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_getReservationStatusesResponse_deserialize(
                adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
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
            AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for getReservationStatusesResponse : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, env);
                      
                    
                    /*
                     * building reservationStatuses array
                     */
                       arr_list = axutil_array_list_create(env, 10);
                   

                     
                     /*
                      * building reservationStatuses element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(env, "reservationStatuses", NULL, NULL);
                                  
                               
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

                                  if (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("reservationStatuses", axiom_element_get_localname(current_element, env)))
                                  {
                                  
                                      is_early_node_valid = AXIS2_TRUE;
                                      
                                     
                                          element = (void*)adb_ReservationStatus_create(env);
                                          
                                          status =  adb_ReservationStatus_deserialize((adb_ReservationStatus_t*)element, env,
                                                                                 &current_node, &is_early_node_valid, AXIS2_FALSE);
                                          
                                          if(AXIS2_FAILURE ==  status)
                                          {
                                              AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building element reservationStatuses ");
                                          }
                                          else
                                          {
                                            axutil_array_list_add_at(arr_list, env, i, element);
                                          }
                                        
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for reservationStatuses ");
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
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "reservationStatuses (@minOccurs = '0') only have %d elements", i);
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
                                    status = adb_getReservationStatusesResponse_set_reservationStatuses(_getReservationStatusesResponse, env,
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
          adb_getReservationStatusesResponse_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_getReservationStatusesResponse_declare_parent_namespaces(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_getReservationStatusesResponse_serialize(
                adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
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
            AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, NULL);
            
            
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
                      

                   if (!_getReservationStatusesResponse->is_valid_reservationStatuses)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("reservationStatuses"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("reservationStatuses")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing reservationStatuses array
                      */
                     if (_getReservationStatusesResponse->property_reservationStatuses != NULL)
                     {
                        

                            sprintf(start_input_str, "<%s%sreservationStatuses",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%sreservationStatuses>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = axutil_array_list_size(_getReservationStatusesResponse->property_reservationStatuses, env);
                         for(i = 0; i < count; i ++)
                         {
                            element = axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i);

                            if(NULL == element) 
                            {
                                continue;
                            }
                    
                     
                     /*
                      * parsing reservationStatuses element
                      */

                    
                     
                            if(!adb_ReservationStatus_is_particle())
                            {
                                axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                            }
                            
                            adb_ReservationStatus_serialize((adb_ReservationStatus_t*)element, 
                                                                                 env, current_node, parent_element,
                                                                                 adb_ReservationStatus_is_particle() || AXIS2_FALSE, namespaces, next_ns_index);
                            
                            if(!adb_ReservationStatus_is_particle())
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
             * getter for reservationStatuses.
             */
            axutil_array_list_t* AXIS2_CALL
            adb_getReservationStatusesResponse_get_reservationStatuses(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, NULL);
                  

                return _getReservationStatusesResponse->property_reservationStatuses;
             }

            /**
             * setter for reservationStatuses
             */
            axis2_status_t AXIS2_CALL
            adb_getReservationStatusesResponse_set_reservationStatuses(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env,
                    axutil_array_list_t*  arg_reservationStatuses)
             {
                
                 int size = 0;
                 int i = 0;
                 axis2_bool_t non_nil_exists = AXIS2_FALSE;
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);
                
                if(_getReservationStatusesResponse->is_valid_reservationStatuses &&
                        arg_reservationStatuses == _getReservationStatusesResponse->property_reservationStatuses)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                 size = axutil_array_list_size(arg_reservationStatuses, env);
                 
                 if (size < 0)
                 {
                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "reservationStatuses has less than minOccurs(0)");
                     return AXIS2_FAILURE;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != axutil_array_list_get(arg_reservationStatuses, env, i))
                     {
                         non_nil_exists = AXIS2_TRUE;
                         break;
                     }
                 }

                 adb_getReservationStatusesResponse_reset_reservationStatuses(_getReservationStatusesResponse, env);

                
                if(NULL == arg_reservationStatuses)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _getReservationStatusesResponse->property_reservationStatuses = arg_reservationStatuses;
                        if(non_nil_exists)
                        {
                            _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_TRUE;
                        }
                        
                    
                return AXIS2_SUCCESS;
             }

            
            /**
             * Get ith element of reservationStatuses.
             */
            adb_ReservationStatus_t* AXIS2_CALL
            adb_getReservationStatusesResponse_get_reservationStatuses_at(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env, int i)
            {
                adb_ReservationStatus_t* ret_val;

                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, NULL);
                  

                if(_getReservationStatusesResponse->property_reservationStatuses == NULL)
                {
                    return (adb_ReservationStatus_t*)0;
                }
                ret_val = (adb_ReservationStatus_t*)axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i);
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of reservationStatuses.
             */
            axis2_status_t AXIS2_CALL
            adb_getReservationStatusesResponse_set_reservationStatuses_at(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env, int i,
                    adb_ReservationStatus_t* arg_reservationStatuses)
            {
                void *element = NULL;
                int size = 0;
                int j;
                int k;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);
                
                if( _getReservationStatusesResponse->is_valid_reservationStatuses &&
                    _getReservationStatusesResponse->property_reservationStatuses &&
                
                    arg_reservationStatuses == (adb_ReservationStatus_t*)axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i))
                  
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                    if(NULL == arg_reservationStatuses)
                    {
                        if(_getReservationStatusesResponse->property_reservationStatuses != NULL)
                        {
                            size = axutil_array_list_size(_getReservationStatusesResponse->property_reservationStatuses, env);
                            for(j = 0, k = 0; j < size; j ++ )
                            {
                                if(i == j) continue; 
                                if(NULL != axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of reservationStatuses is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }

                if(_getReservationStatusesResponse->property_reservationStatuses == NULL)
                {
                    _getReservationStatusesResponse->property_reservationStatuses = axutil_array_list_create(env, 10);
                }
                
                /* check whether there already exist an element */
                element = axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i);
                if(NULL != element)
                {
                  
                  
                  
                        adb_ReservationStatus_free((adb_ReservationStatus_t*)element, env);
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_FALSE;
                        axutil_array_list_set(_getReservationStatusesResponse->property_reservationStatuses , env, i, NULL);
                        
                        return AXIS2_SUCCESS;
                    }
                
                   axutil_array_list_set(_getReservationStatusesResponse->property_reservationStatuses , env, i, arg_reservationStatuses);
                  _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_TRUE;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to reservationStatuses.
             */
            axis2_status_t AXIS2_CALL
            adb_getReservationStatusesResponse_add_reservationStatuses(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env,
                    adb_ReservationStatus_t* arg_reservationStatuses)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);

                
                    if(NULL == arg_reservationStatuses)
                    {
                      
                           return AXIS2_SUCCESS; 
                        
                    }
                  

                if(_getReservationStatusesResponse->property_reservationStatuses == NULL)
                {
                    _getReservationStatusesResponse->property_reservationStatuses = axutil_array_list_create(env, 10);
                }
                if(_getReservationStatusesResponse->property_reservationStatuses == NULL)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Failed in allocatting memory for reservationStatuses");
                    return AXIS2_FAILURE;
                    
                }
                
                   axutil_array_list_add(_getReservationStatusesResponse->property_reservationStatuses , env, arg_reservationStatuses);
                  _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_TRUE;
                return AXIS2_SUCCESS;
             }

            /**
             * Get the size of the reservationStatuses array.
             */
            int AXIS2_CALL
            adb_getReservationStatusesResponse_sizeof_reservationStatuses(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env)
            {
                AXIS2_ENV_CHECK(env, -1);
                AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, -1);
                if(_getReservationStatusesResponse->property_reservationStatuses == NULL)
                {
                    return 0;
                }
                return axutil_array_list_size(_getReservationStatusesResponse->property_reservationStatuses, env);
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            axis2_status_t AXIS2_CALL
            adb_getReservationStatusesResponse_remove_reservationStatuses_at(
                    adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                    const axutil_env_t *env, int i)
            {
                return adb_getReservationStatusesResponse_set_reservationStatuses_nil_at(_getReservationStatusesResponse, env, i);
            }

            

           /**
            * resetter for reservationStatuses
            */
           axis2_status_t AXIS2_CALL
           adb_getReservationStatusesResponse_reset_reservationStatuses(
                   adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);
               

               
                  if (_getReservationStatusesResponse->property_reservationStatuses != NULL)
                  {
                      count = axutil_array_list_size(_getReservationStatusesResponse->property_reservationStatuses, env);
                      for(i = 0; i < count; i ++)
                      {
                         element = axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i);
                
            
                
                if(element != NULL)
                {
                   
                   
                        adb_ReservationStatus_free((adb_ReservationStatus_t*)element, env);
                     element = NULL;
                }
            
                
                
                
                      }
                      axutil_array_list_free(_getReservationStatusesResponse->property_reservationStatuses, env);
                  }
                _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether reservationStatuses is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_getReservationStatusesResponse_is_reservationStatuses_nil(
                   adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_TRUE);
               
               return !_getReservationStatusesResponse->is_valid_reservationStatuses;
           }

           /**
            * Set reservationStatuses to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_getReservationStatusesResponse_set_reservationStatuses_nil(
                   adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                   const axutil_env_t *env)
           {
               return adb_getReservationStatusesResponse_reset_reservationStatuses(_getReservationStatusesResponse, env);
           }

           
           /**
            * Check whether reservationStatuses is nill at i
            */
           axis2_bool_t AXIS2_CALL
           adb_getReservationStatusesResponse_is_reservationStatuses_nil_at(
                   adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                   const axutil_env_t *env, int i)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_TRUE);
               
               return (_getReservationStatusesResponse->is_valid_reservationStatuses == AXIS2_FALSE ||
                        NULL == _getReservationStatusesResponse->property_reservationStatuses || 
                        NULL == axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i));
           }

           /**
            * Set reservationStatuses to nill at i
            */
           axis2_status_t AXIS2_CALL
           adb_getReservationStatusesResponse_set_reservationStatuses_nil_at(
                   adb_getReservationStatusesResponse_t* _getReservationStatusesResponse,
                   const axutil_env_t *env, int i)
           {
                void *element = NULL;
                int size = 0;
                int j;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                int k = 0;

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getReservationStatusesResponse, AXIS2_FAILURE);

                if(_getReservationStatusesResponse->property_reservationStatuses == NULL ||
                            _getReservationStatusesResponse->is_valid_reservationStatuses == AXIS2_FALSE)
                {
                    
                    non_nil_exists = AXIS2_FALSE;
                }
                else
                {
                    size = axutil_array_list_size(_getReservationStatusesResponse->property_reservationStatuses, env);
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of reservationStatuses is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }
 
                if(_getReservationStatusesResponse->property_reservationStatuses == NULL)
                {
                    _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_FALSE;
                    
                    return AXIS2_SUCCESS;
                }

                /* check whether there already exist an element */
                element = axutil_array_list_get(_getReservationStatusesResponse->property_reservationStatuses, env, i);
                if(NULL != element)
                {
                  
                  
                  
                        adb_ReservationStatus_free((adb_ReservationStatus_t*)element, env);
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _getReservationStatusesResponse->is_valid_reservationStatuses = AXIS2_FALSE;
                        axutil_array_list_set(_getReservationStatusesResponse->property_reservationStatuses , env, i, NULL);
                        return AXIS2_SUCCESS;
                    }
                

                
                axutil_array_list_set(_getReservationStatusesResponse->property_reservationStatuses , env, i, NULL);
                
                return AXIS2_SUCCESS;

           }

           

