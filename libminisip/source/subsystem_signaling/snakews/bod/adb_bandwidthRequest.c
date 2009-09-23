

        /**
         * adb_bandwidthRequest.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_bandwidthRequest.h"
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = bandwidthRequest
                 * Namespace URI = http://esb.bod.services.hdviper.i2cat.net/
                 * Namespace Prefix = ns1
                 */
           


        struct adb_bandwidthRequest
        {
            int property_bandwidth;

                
                axis2_bool_t is_valid_bandwidth;


            axutil_array_list_t* property_destinationIP;

                
                axis2_bool_t is_valid_destinationIP;


            axis2_char_t* property_destinationSipAddress;

                
                axis2_bool_t is_valid_destinationSipAddress;


            axutil_date_time_t* property_end;

                
                axis2_bool_t is_valid_end;


            axutil_array_list_t* property_sourceIP;

                
                axis2_bool_t is_valid_sourceIP;


            axis2_char_t* property_sourceSipAddress;

                
                axis2_bool_t is_valid_sourceSipAddress;


            axutil_date_time_t* property_start;

                
                axis2_bool_t is_valid_start;


            
        };


       /************************* Private Function prototypes ********************************/
        

                axis2_status_t AXIS2_CALL
                adb_bandwidthRequest_set_bandwidth_nil(
                        adb_bandwidthRequest_t* _bandwidthRequest,
                        const axutil_env_t *env);
            

                axis2_status_t AXIS2_CALL
                adb_bandwidthRequest_set_destinationSipAddress_nil(
                        adb_bandwidthRequest_t* _bandwidthRequest,
                        const axutil_env_t *env);
            

                axis2_status_t AXIS2_CALL
                adb_bandwidthRequest_set_end_nil(
                        adb_bandwidthRequest_t* _bandwidthRequest,
                        const axutil_env_t *env);
            

                axis2_status_t AXIS2_CALL
                adb_bandwidthRequest_set_sourceSipAddress_nil(
                        adb_bandwidthRequest_t* _bandwidthRequest,
                        const axutil_env_t *env);
            

                axis2_status_t AXIS2_CALL
                adb_bandwidthRequest_set_start_nil(
                        adb_bandwidthRequest_t* _bandwidthRequest,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_bandwidthRequest_t* AXIS2_CALL
        adb_bandwidthRequest_create(
            const axutil_env_t *env)
        {
            adb_bandwidthRequest_t *_bandwidthRequest = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _bandwidthRequest = (adb_bandwidthRequest_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_bandwidthRequest_t));

            if(NULL == _bandwidthRequest)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_bandwidthRequest, 0, sizeof(adb_bandwidthRequest_t));

            _bandwidthRequest->is_valid_bandwidth  = AXIS2_FALSE;
            _bandwidthRequest->property_destinationIP  = NULL;
                  _bandwidthRequest->is_valid_destinationIP  = AXIS2_FALSE;
            _bandwidthRequest->property_destinationSipAddress  = NULL;
                  _bandwidthRequest->is_valid_destinationSipAddress  = AXIS2_FALSE;
            _bandwidthRequest->property_end  = NULL;
                  _bandwidthRequest->is_valid_end  = AXIS2_FALSE;
            _bandwidthRequest->property_sourceIP  = NULL;
                  _bandwidthRequest->is_valid_sourceIP  = AXIS2_FALSE;
            _bandwidthRequest->property_sourceSipAddress  = NULL;
                  _bandwidthRequest->is_valid_sourceSipAddress  = AXIS2_FALSE;
            _bandwidthRequest->property_start  = NULL;
                  _bandwidthRequest->is_valid_start  = AXIS2_FALSE;
            

            return _bandwidthRequest;
        }

        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_free (
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env)
        {
            
                int i = 0;
                int count = 0;
                void *element = NULL;
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);

            adb_bandwidthRequest_reset_bandwidth(_bandwidthRequest, env);
            adb_bandwidthRequest_reset_destinationIP(_bandwidthRequest, env);
            adb_bandwidthRequest_reset_destinationSipAddress(_bandwidthRequest, env);
            adb_bandwidthRequest_reset_end(_bandwidthRequest, env);
            adb_bandwidthRequest_reset_sourceIP(_bandwidthRequest, env);
            adb_bandwidthRequest_reset_sourceSipAddress(_bandwidthRequest, env);
            adb_bandwidthRequest_reset_start(_bandwidthRequest, env);
            

            if(_bandwidthRequest)
            {
                AXIS2_FREE(env->allocator, _bandwidthRequest);
                _bandwidthRequest = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_deserialize(
                adb_bandwidthRequest_t* _bandwidthRequest,
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
            AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for bandwidthRequest : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, env);
                      
                    

                     
                     /*
                      * building bandwidth element
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
                                   
                                 element_qname = axutil_qname_create(env, "bandwidth", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("bandwidth", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("bandwidth", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                            status = adb_bandwidthRequest_set_bandwidth(_bandwidthRequest, env,
                                                                   atoi(text_value));
                                      }
                                      
                                      else
                                      {
                                          AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element bandwidth");
                                          status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for bandwidth ");
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
                                  AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "non nillable or minOuccrs != 0 element bandwidth missing");
                                  return AXIS2_FAILURE;
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 
                    /*
                     * building destinationIP array
                     */
                       arr_list = axutil_array_list_create(env, 10);
                   

                     
                     /*
                      * building destinationIP element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(env, "destinationIP", NULL, NULL);
                                  
                               
                               for (i = 0, sequence_broken = 0, current_node = (is_early_node_valid?axiom_node_get_next_sibling(current_node, env):current_node); !sequence_broken && current_node != NULL;) 
                                             
                               {
                                  if(axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                  {
                                     current_node =axiom_node_get_next_sibling(current_node, env);
                                     is_early_node_valid = AXIS2_FALSE;
                                     continue;
                                  }
                                  
                                  current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                  qname = axiom_element_get_qname(current_element, env, current_node);

                                  if (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("destinationIP", axiom_element_get_localname(current_element, env)))
                                  {
                                  
                                      is_early_node_valid = AXIS2_TRUE;
                                      
                                     
                                          text_value = axiom_element_get_text(current_element, env, current_node);
                                          if(text_value != NULL)
                                          {
                                              axutil_array_list_add_at(arr_list, env, i, axutil_strdup(env, text_value));
                                          }
                                          
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for destinationIP ");
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
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "destinationIP (@minOccurs = '0') only have %d elements", i);
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
                                    status = adb_bandwidthRequest_set_destinationIP(_bandwidthRequest, env,
                                                                   arr_list);
                               }

                             
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building destinationSipAddress element
                      */
                     
                     
                     
                                    /*
                                     * because elements are ordered this works fine
                                     */
                                  
                                   
                                   if(current_node != NULL && is_early_node_valid)
                                   {
                                       current_node = axiom_node_get_next_sibling(current_node, env);
                                       
                                       
                                        while(current_node && axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                        {
                                            current_node = axiom_node_get_next_sibling(current_node, env);
                                        }
                                        if(current_node != NULL)
                                        {
                                            current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                            qname = axiom_element_get_qname(current_element, env, current_node);
                                        }
                                       
                                   }
                                   is_early_node_valid = AXIS2_FALSE;
                                 
                                 element_qname = axutil_qname_create(env, "destinationSipAddress", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("destinationSipAddress", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("destinationSipAddress", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                            status = adb_bandwidthRequest_set_destinationSipAddress(_bandwidthRequest, env,
                                                               text_value);
                                      }
                                      
                                      else
                                      {
                                            /*
                                             * axis2_qname_t *qname = NULL;
                                             * axiom_attribute_t *the_attri = NULL;
                                             * 
                                             * qname = axutil_qname_create(env, "nil", "http://www.w3.org/2001/XMLSchema-instance", "xsi");
                                             * the_attri = axiom_element_get_attribute(current_element, env, qname);
                                             */
                                            /* currently thereis a bug in the axiom_element_get_attribute, so we have to go to this bad method */

                                            axiom_attribute_t *the_attri = NULL;
                                            axis2_char_t *attrib_text = NULL;
                                            axutil_hash_t *attribute_hash = NULL;

                                            attribute_hash = axiom_element_get_all_attributes(current_element, env);

                                            attrib_text = NULL;
                                            if(attribute_hash)
                                            {
                                                 axutil_hash_index_t *hi;
                                                 void *val;
                                                 const void *key;
                                        
                                                 for (hi = axutil_hash_first(attribute_hash, env); hi; hi = axutil_hash_next(env, hi)) 
                                                 {
                                                     axutil_hash_this(hi, &key, NULL, &val);
                                                     
                                                     if(strstr((axis2_char_t*)key, "nil|http://www.w3.org/2001/XMLSchema-instance"))
                                                     {
                                                         the_attri = (axiom_attribute_t*)val;
                                                         break;
                                                     }
                                                 }
                                            }

                                            if(the_attri)
                                            {
                                                attrib_text = axiom_attribute_get_value(the_attri, env);
                                            }
                                            else
                                            {
                                                /* this is hoping that attribute is stored in "http://www.w3.org/2001/XMLSchema-instance", this happnes when name is in default namespace */
                                                attrib_text = axiom_element_get_attribute_value_by_name(current_element, env, "nil");
                                            }

                                            if(attrib_text && 0 == axutil_strcmp(attrib_text, "1"))
                                            {
                                                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element destinationSipAddress");
                                                status = AXIS2_FAILURE;
                                            }
                                            else
                                            {
                                                /* after all, we found this is a empty string */
                                                status = adb_bandwidthRequest_set_destinationSipAddress(_bandwidthRequest, env,
                                                                   "");
                                            }
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for destinationSipAddress ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, env);
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building end element
                      */
                     
                     
                     
                                    /*
                                     * because elements are ordered this works fine
                                     */
                                  
                                   
                                   if(current_node != NULL && is_early_node_valid)
                                   {
                                       current_node = axiom_node_get_next_sibling(current_node, env);
                                       
                                       
                                        while(current_node && axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                        {
                                            current_node = axiom_node_get_next_sibling(current_node, env);
                                        }
                                        if(current_node != NULL)
                                        {
                                            current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                            qname = axiom_element_get_qname(current_element, env, current_node);
                                        }
                                       
                                   }
                                   is_early_node_valid = AXIS2_FALSE;
                                 
                                 element_qname = axutil_qname_create(env, "end", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("end", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("end", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                          element = (void*)axutil_date_time_create(env);
                                          status = axutil_date_time_deserialize_date_time((axutil_date_time_t*)element, env,
                                                                          text_value);
                                          if(AXIS2_FAILURE ==  status)
                                          {
                                              if(element != NULL)
                                              {
                                                  axutil_date_time_free((axutil_date_time_t*)element, env);
                                              }
                                              AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building element end ");
                                          }
                                          else
                                          {
                                            status = adb_bandwidthRequest_set_end(_bandwidthRequest, env,
                                                                       (axutil_date_time_t*)element);
                                          }
                                      }
                                      
                                      else
                                      {
                                          AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element end");
                                          status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for end ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, env);
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 
                    /*
                     * building sourceIP array
                     */
                       arr_list = axutil_array_list_create(env, 10);
                   

                     
                     /*
                      * building sourceIP element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(env, "sourceIP", NULL, NULL);
                                  
                               
                               for (i = 0, sequence_broken = 0, current_node = (is_early_node_valid?axiom_node_get_next_sibling(current_node, env):current_node); !sequence_broken && current_node != NULL;) 
                                             
                               {
                                  if(axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                  {
                                     current_node =axiom_node_get_next_sibling(current_node, env);
                                     is_early_node_valid = AXIS2_FALSE;
                                     continue;
                                  }
                                  
                                  current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                  qname = axiom_element_get_qname(current_element, env, current_node);

                                  if (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("sourceIP", axiom_element_get_localname(current_element, env)))
                                  {
                                  
                                      is_early_node_valid = AXIS2_TRUE;
                                      
                                     
                                          text_value = axiom_element_get_text(current_element, env, current_node);
                                          if(text_value != NULL)
                                          {
                                              axutil_array_list_add_at(arr_list, env, i, axutil_strdup(env, text_value));
                                          }
                                          
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for sourceIP ");
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
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sourceIP (@minOccurs = '0') only have %d elements", i);
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
                                    status = adb_bandwidthRequest_set_sourceIP(_bandwidthRequest, env,
                                                                   arr_list);
                               }

                             
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building sourceSipAddress element
                      */
                     
                     
                     
                                    /*
                                     * because elements are ordered this works fine
                                     */
                                  
                                   
                                   if(current_node != NULL && is_early_node_valid)
                                   {
                                       current_node = axiom_node_get_next_sibling(current_node, env);
                                       
                                       
                                        while(current_node && axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                        {
                                            current_node = axiom_node_get_next_sibling(current_node, env);
                                        }
                                        if(current_node != NULL)
                                        {
                                            current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                            qname = axiom_element_get_qname(current_element, env, current_node);
                                        }
                                       
                                   }
                                   is_early_node_valid = AXIS2_FALSE;
                                 
                                 element_qname = axutil_qname_create(env, "sourceSipAddress", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("sourceSipAddress", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("sourceSipAddress", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                            status = adb_bandwidthRequest_set_sourceSipAddress(_bandwidthRequest, env,
                                                               text_value);
                                      }
                                      
                                      else
                                      {
                                            /*
                                             * axis2_qname_t *qname = NULL;
                                             * axiom_attribute_t *the_attri = NULL;
                                             * 
                                             * qname = axutil_qname_create(env, "nil", "http://www.w3.org/2001/XMLSchema-instance", "xsi");
                                             * the_attri = axiom_element_get_attribute(current_element, env, qname);
                                             */
                                            /* currently thereis a bug in the axiom_element_get_attribute, so we have to go to this bad method */

                                            axiom_attribute_t *the_attri = NULL;
                                            axis2_char_t *attrib_text = NULL;
                                            axutil_hash_t *attribute_hash = NULL;

                                            attribute_hash = axiom_element_get_all_attributes(current_element, env);

                                            attrib_text = NULL;
                                            if(attribute_hash)
                                            {
                                                 axutil_hash_index_t *hi;
                                                 void *val;
                                                 const void *key;
                                        
                                                 for (hi = axutil_hash_first(attribute_hash, env); hi; hi = axutil_hash_next(env, hi)) 
                                                 {
                                                     axutil_hash_this(hi, &key, NULL, &val);
                                                     
                                                     if(strstr((axis2_char_t*)key, "nil|http://www.w3.org/2001/XMLSchema-instance"))
                                                     {
                                                         the_attri = (axiom_attribute_t*)val;
                                                         break;
                                                     }
                                                 }
                                            }

                                            if(the_attri)
                                            {
                                                attrib_text = axiom_attribute_get_value(the_attri, env);
                                            }
                                            else
                                            {
                                                /* this is hoping that attribute is stored in "http://www.w3.org/2001/XMLSchema-instance", this happnes when name is in default namespace */
                                                attrib_text = axiom_element_get_attribute_value_by_name(current_element, env, "nil");
                                            }

                                            if(attrib_text && 0 == axutil_strcmp(attrib_text, "1"))
                                            {
                                                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element sourceSipAddress");
                                                status = AXIS2_FAILURE;
                                            }
                                            else
                                            {
                                                /* after all, we found this is a empty string */
                                                status = adb_bandwidthRequest_set_sourceSipAddress(_bandwidthRequest, env,
                                                                   "");
                                            }
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for sourceSipAddress ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, env);
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 

                     
                     /*
                      * building start element
                      */
                     
                     
                     
                                    /*
                                     * because elements are ordered this works fine
                                     */
                                  
                                   
                                   if(current_node != NULL && is_early_node_valid)
                                   {
                                       current_node = axiom_node_get_next_sibling(current_node, env);
                                       
                                       
                                        while(current_node && axiom_node_get_node_type(current_node, env) != AXIOM_ELEMENT)
                                        {
                                            current_node = axiom_node_get_next_sibling(current_node, env);
                                        }
                                        if(current_node != NULL)
                                        {
                                            current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, env);
                                            qname = axiom_element_get_qname(current_element, env, current_node);
                                        }
                                       
                                   }
                                   is_early_node_valid = AXIS2_FALSE;
                                 
                                 element_qname = axutil_qname_create(env, "start", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("start", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("start", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                          element = (void*)axutil_date_time_create(env);
                                          status = axutil_date_time_deserialize_date_time((axutil_date_time_t*)element, env,
                                                                          text_value);
                                          if(AXIS2_FAILURE ==  status)
                                          {
                                              if(element != NULL)
                                              {
                                                  axutil_date_time_free((axutil_date_time_t*)element, env);
                                              }
                                              AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building element start ");
                                          }
                                          else
                                          {
                                            status = adb_bandwidthRequest_set_start(_bandwidthRequest, env,
                                                                       (axutil_date_time_t*)element);
                                          }
                                      }
                                      
                                      else
                                      {
                                          AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element start");
                                          status = AXIS2_FAILURE;
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for start ");
                                     if(element_qname)
                                     {
                                         axutil_qname_free(element_qname, env);
                                     }
                                     return AXIS2_FAILURE;
                                 }
                              }
                           
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, env);
                     element_qname = NULL;
                  }
                 
          return status;
       }

          axis2_bool_t AXIS2_CALL
          adb_bandwidthRequest_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_bandwidthRequest_declare_parent_namespaces(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_bandwidthRequest_serialize(
                adb_bandwidthRequest_t* _bandwidthRequest,
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
                    
                    axis2_char_t *text_value_2;
                    axis2_char_t *text_value_2_temp;
                    
                    axis2_char_t *text_value_3;
                    axis2_char_t *text_value_3_temp;
                    
                    axis2_char_t *text_value_4;
                    axis2_char_t *text_value_4_temp;
                    
                    axis2_char_t *text_value_5;
                    axis2_char_t *text_value_5_temp;
                    
                    axis2_char_t *text_value_6;
                    axis2_char_t *text_value_6_temp;
                    
                    axis2_char_t *text_value_7;
                    axis2_char_t *text_value_7_temp;
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
            
            
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
                      

                   if (!_bandwidthRequest->is_valid_bandwidth)
                   {
                      
                            
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Nil value found in non-nillable property bandwidth");
                            return NULL;
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("bandwidth"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("bandwidth")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing bandwidth element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sbandwidth>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sbandwidth>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                               sprintf (text_value_1, AXIS2_PRINTF_INT32_FORMAT_SPECIFIER, _bandwidthRequest->property_bandwidth);
                             
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, env, text_value_1, axutil_strlen(text_value_1));
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_bandwidthRequest->is_valid_destinationIP)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("destinationIP"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("destinationIP")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing destinationIP array
                      */
                     if (_bandwidthRequest->property_destinationIP != NULL)
                     {
                        
                            sprintf(start_input_str, "<%s%sdestinationIP>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%sdestinationIP>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = axutil_array_list_size(_bandwidthRequest->property_destinationIP, env);
                         for(i = 0; i < count; i ++)
                         {
                            element = axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i);

                            if(NULL == element) 
                            {
                                continue;
                            }
                    
                     
                     /*
                      * parsing destinationIP element
                      */

                    
                    
                           text_value_2 = (axis2_char_t*)element;
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                            
                           text_value_2_temp = axutil_xml_quote_string(env, text_value_2, AXIS2_TRUE);
                           if (text_value_2_temp)
                           {
                               axutil_stream_write(stream, env, text_value_2_temp, axutil_strlen(text_value_2_temp));
                               AXIS2_FREE(env->allocator, text_value_2_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, env, text_value_2, axutil_strlen(text_value_2));
                           }
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                         }
                     }
                   
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_bandwidthRequest->is_valid_destinationSipAddress)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("destinationSipAddress"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("destinationSipAddress")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing destinationSipAddress element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sdestinationSipAddress>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sdestinationSipAddress>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                           text_value_3 = _bandwidthRequest->property_destinationSipAddress;
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                            
                           text_value_3_temp = axutil_xml_quote_string(env, text_value_3, AXIS2_TRUE);
                           if (text_value_3_temp)
                           {
                               axutil_stream_write(stream, env, text_value_3_temp, axutil_strlen(text_value_3_temp));
                               AXIS2_FREE(env->allocator, text_value_3_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, env, text_value_3, axutil_strlen(text_value_3));
                           }
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_bandwidthRequest->is_valid_end)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("end"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("end")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing end element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%send>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%send>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                          text_value_4 = axutil_date_time_serialize_date_time(_bandwidthRequest->property_end, env);
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, env, text_value_4, axutil_strlen(text_value_4));
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_bandwidthRequest->is_valid_sourceIP)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("sourceIP"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("sourceIP")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing sourceIP array
                      */
                     if (_bandwidthRequest->property_sourceIP != NULL)
                     {
                        
                            sprintf(start_input_str, "<%s%ssourceIP>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%ssourceIP>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = axutil_array_list_size(_bandwidthRequest->property_sourceIP, env);
                         for(i = 0; i < count; i ++)
                         {
                            element = axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i);

                            if(NULL == element) 
                            {
                                continue;
                            }
                    
                     
                     /*
                      * parsing sourceIP element
                      */

                    
                    
                           text_value_5 = (axis2_char_t*)element;
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                            
                           text_value_5_temp = axutil_xml_quote_string(env, text_value_5, AXIS2_TRUE);
                           if (text_value_5_temp)
                           {
                               axutil_stream_write(stream, env, text_value_5_temp, axutil_strlen(text_value_5_temp));
                               AXIS2_FREE(env->allocator, text_value_5_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, env, text_value_5, axutil_strlen(text_value_5));
                           }
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                         }
                     }
                   
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_bandwidthRequest->is_valid_sourceSipAddress)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("sourceSipAddress"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("sourceSipAddress")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing sourceSipAddress element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%ssourceSipAddress>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%ssourceSipAddress>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                           text_value_6 = _bandwidthRequest->property_sourceSipAddress;
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                            
                           text_value_6_temp = axutil_xml_quote_string(env, text_value_6, AXIS2_TRUE);
                           if (text_value_6_temp)
                           {
                               axutil_stream_write(stream, env, text_value_6_temp, axutil_strlen(text_value_6_temp));
                               AXIS2_FREE(env->allocator, text_value_6_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, env, text_value_6, axutil_strlen(text_value_6));
                           }
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_bandwidthRequest->is_valid_start)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("start"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("start")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing start element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sstart>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sstart>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                          text_value_7 = axutil_date_time_serialize_date_time(_bandwidthRequest->property_start, env);
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                           axutil_stream_write(stream, env, text_value_7, axutil_strlen(text_value_7));
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 

            return parent;
        }


        

            /**
             * getter for bandwidth.
             */
            int AXIS2_CALL
            adb_bandwidthRequest_get_bandwidth(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, (int)0);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, (int)0);
                  

                return _bandwidthRequest->property_bandwidth;
             }

            /**
             * setter for bandwidth
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_bandwidth(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    const int  arg_bandwidth)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_bandwidth &&
                        arg_bandwidth == _bandwidthRequest->property_bandwidth)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_bandwidthRequest_reset_bandwidth(_bandwidthRequest, env);

                _bandwidthRequest->property_bandwidth = arg_bandwidth;
                        _bandwidthRequest->is_valid_bandwidth = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for bandwidth
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_bandwidth(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               _bandwidthRequest->is_valid_bandwidth = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether bandwidth is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_bandwidth_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_bandwidth;
           }

           /**
            * Set bandwidth to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_bandwidth_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_bandwidth(_bandwidthRequest, env);
           }

           

            /**
             * getter for destinationIP.
             */
            axutil_array_list_t* AXIS2_CALL
            adb_bandwidthRequest_get_destinationIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                return _bandwidthRequest->property_destinationIP;
             }

            /**
             * setter for destinationIP
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_destinationIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    axutil_array_list_t*  arg_destinationIP)
             {
                
                 int size = 0;
                 int i = 0;
                 axis2_bool_t non_nil_exists = AXIS2_FALSE;
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_destinationIP &&
                        arg_destinationIP == _bandwidthRequest->property_destinationIP)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                 size = axutil_array_list_size(arg_destinationIP, env);
                 
                 if (size < 0)
                 {
                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "destinationIP has less than minOccurs(0)");
                     return AXIS2_FAILURE;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != axutil_array_list_get(arg_destinationIP, env, i))
                     {
                         non_nil_exists = AXIS2_TRUE;
                         break;
                     }
                 }

                 adb_bandwidthRequest_reset_destinationIP(_bandwidthRequest, env);

                
                if(NULL == arg_destinationIP)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _bandwidthRequest->property_destinationIP = arg_destinationIP;
                        if(non_nil_exists)
                        {
                            _bandwidthRequest->is_valid_destinationIP = AXIS2_TRUE;
                        }
                        
                    
                return AXIS2_SUCCESS;
             }

            
            /**
             * Get ith element of destinationIP.
             */
            axis2_char_t* AXIS2_CALL
            adb_bandwidthRequest_get_destinationIP_at(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, int i)
            {
                axis2_char_t* ret_val;

                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                if(_bandwidthRequest->property_destinationIP == NULL)
                {
                    return (axis2_char_t*)0;
                }
                ret_val = (axis2_char_t*)axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i);
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of destinationIP.
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_destinationIP_at(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, int i,
                    const axis2_char_t* arg_destinationIP)
            {
                void *element = NULL;
                int size = 0;
                int j;
                int k;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if( _bandwidthRequest->is_valid_destinationIP &&
                    _bandwidthRequest->property_destinationIP &&
                
                    arg_destinationIP == (axis2_char_t*)axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i))
                  
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                    if(NULL == arg_destinationIP)
                    {
                        if(_bandwidthRequest->property_destinationIP != NULL)
                        {
                            size = axutil_array_list_size(_bandwidthRequest->property_destinationIP, env);
                            for(j = 0, k = 0; j < size; j ++ )
                            {
                                if(i == j) continue; 
                                if(NULL != axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of destinationIP is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }

                if(_bandwidthRequest->property_destinationIP == NULL)
                {
                    _bandwidthRequest->property_destinationIP = axutil_array_list_create(env, 10);
                }
                
                /* check whether there already exist an element */
                element = axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i);
                if(NULL != element)
                {
                  
                  
                  
                       /* This is an unknown type or a primitive. Please free this manually*/
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _bandwidthRequest->is_valid_destinationIP = AXIS2_FALSE;
                        axutil_array_list_set(_bandwidthRequest->property_destinationIP , env, i, NULL);
                        
                        return AXIS2_SUCCESS;
                    }
                
                   axutil_array_list_set(_bandwidthRequest->property_destinationIP , env, i, axutil_strdup(env, arg_destinationIP));
                  _bandwidthRequest->is_valid_destinationIP = AXIS2_TRUE;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to destinationIP.
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_add_destinationIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    const axis2_char_t* arg_destinationIP)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);

                
                    if(NULL == arg_destinationIP)
                    {
                      
                           return AXIS2_SUCCESS; 
                        
                    }
                  

                if(_bandwidthRequest->property_destinationIP == NULL)
                {
                    _bandwidthRequest->property_destinationIP = axutil_array_list_create(env, 10);
                }
                if(_bandwidthRequest->property_destinationIP == NULL)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Failed in allocatting memory for destinationIP");
                    return AXIS2_FAILURE;
                    
                }
                
                   axutil_array_list_add(_bandwidthRequest->property_destinationIP , env, axutil_strdup(env, arg_destinationIP));
                  _bandwidthRequest->is_valid_destinationIP = AXIS2_TRUE;
                return AXIS2_SUCCESS;
             }

            /**
             * Get the size of the destinationIP array.
             */
            int AXIS2_CALL
            adb_bandwidthRequest_sizeof_destinationIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
            {
                AXIS2_ENV_CHECK(env, -1);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, -1);
                if(_bandwidthRequest->property_destinationIP == NULL)
                {
                    return 0;
                }
                return axutil_array_list_size(_bandwidthRequest->property_destinationIP, env);
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_remove_destinationIP_at(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, int i)
            {
                return adb_bandwidthRequest_set_destinationIP_nil_at(_bandwidthRequest, env, i);
            }

            

           /**
            * resetter for destinationIP
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_destinationIP(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               
                  if (_bandwidthRequest->property_destinationIP != NULL)
                  {
                      count = axutil_array_list_size(_bandwidthRequest->property_destinationIP, env);
                      for(i = 0; i < count; i ++)
                      {
                         element = axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i);
                
            
                
                if(element != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, (axis2_char_t*)element);
                     element = NULL;
                }
            
                
                
                
                      }
                      axutil_array_list_free(_bandwidthRequest->property_destinationIP, env);
                  }
                _bandwidthRequest->is_valid_destinationIP = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether destinationIP is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_destinationIP_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_destinationIP;
           }

           /**
            * Set destinationIP to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_destinationIP_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_destinationIP(_bandwidthRequest, env);
           }

           
           /**
            * Check whether destinationIP is nill at i
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_destinationIP_nil_at(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env, int i)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return (_bandwidthRequest->is_valid_destinationIP == AXIS2_FALSE ||
                        NULL == _bandwidthRequest->property_destinationIP || 
                        NULL == axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i));
           }

           /**
            * Set destinationIP to nill at i
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_destinationIP_nil_at(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env, int i)
           {
                void *element = NULL;
                int size = 0;
                int j;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                int k = 0;

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);

                if(_bandwidthRequest->property_destinationIP == NULL ||
                            _bandwidthRequest->is_valid_destinationIP == AXIS2_FALSE)
                {
                    
                    non_nil_exists = AXIS2_FALSE;
                }
                else
                {
                    size = axutil_array_list_size(_bandwidthRequest->property_destinationIP, env);
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of destinationIP is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }
 
                if(_bandwidthRequest->property_destinationIP == NULL)
                {
                    _bandwidthRequest->is_valid_destinationIP = AXIS2_FALSE;
                    
                    return AXIS2_SUCCESS;
                }

                /* check whether there already exist an element */
                element = axutil_array_list_get(_bandwidthRequest->property_destinationIP, env, i);
                if(NULL != element)
                {
                  
                  
                  
                       /* This is an unknown type or a primitive. Please free this manually*/
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _bandwidthRequest->is_valid_destinationIP = AXIS2_FALSE;
                        axutil_array_list_set(_bandwidthRequest->property_destinationIP , env, i, NULL);
                        return AXIS2_SUCCESS;
                    }
                

                
                axutil_array_list_set(_bandwidthRequest->property_destinationIP , env, i, NULL);
                
                return AXIS2_SUCCESS;

           }

           

            /**
             * getter for destinationSipAddress.
             */
            axis2_char_t* AXIS2_CALL
            adb_bandwidthRequest_get_destinationSipAddress(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                return _bandwidthRequest->property_destinationSipAddress;
             }

            /**
             * setter for destinationSipAddress
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_destinationSipAddress(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    const axis2_char_t*  arg_destinationSipAddress)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_destinationSipAddress &&
                        arg_destinationSipAddress == _bandwidthRequest->property_destinationSipAddress)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_bandwidthRequest_reset_destinationSipAddress(_bandwidthRequest, env);

                
                if(NULL == arg_destinationSipAddress)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _bandwidthRequest->property_destinationSipAddress = (axis2_char_t *)axutil_strdup(env, arg_destinationSipAddress);
                        if(NULL == _bandwidthRequest->property_destinationSipAddress)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Error allocating memeory for destinationSipAddress");
                            return AXIS2_FAILURE;
                        }
                        _bandwidthRequest->is_valid_destinationSipAddress = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for destinationSipAddress
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_destinationSipAddress(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               
            
                
                if(_bandwidthRequest->property_destinationSipAddress != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, _bandwidthRequest->property_destinationSipAddress);
                     _bandwidthRequest->property_destinationSipAddress = NULL;
                }
            
                
                
                _bandwidthRequest->is_valid_destinationSipAddress = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether destinationSipAddress is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_destinationSipAddress_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_destinationSipAddress;
           }

           /**
            * Set destinationSipAddress to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_destinationSipAddress_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_destinationSipAddress(_bandwidthRequest, env);
           }

           

            /**
             * getter for end.
             */
            axutil_date_time_t* AXIS2_CALL
            adb_bandwidthRequest_get_end(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                return _bandwidthRequest->property_end;
             }

            /**
             * setter for end
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_end(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    axutil_date_time_t*  arg_end)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_end &&
                        arg_end == _bandwidthRequest->property_end)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_bandwidthRequest_reset_end(_bandwidthRequest, env);

                
                if(NULL == arg_end)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _bandwidthRequest->property_end = arg_end;
                        _bandwidthRequest->is_valid_end = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for end
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_end(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               
            
                
                if(_bandwidthRequest->property_end != NULL)
                {
                   
                   
                      axutil_date_time_free(_bandwidthRequest->property_end, env);
                     _bandwidthRequest->property_end = NULL;
                }
            
                
                
                _bandwidthRequest->is_valid_end = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether end is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_end_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_end;
           }

           /**
            * Set end to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_end_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_end(_bandwidthRequest, env);
           }

           

            /**
             * getter for sourceIP.
             */
            axutil_array_list_t* AXIS2_CALL
            adb_bandwidthRequest_get_sourceIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                return _bandwidthRequest->property_sourceIP;
             }

            /**
             * setter for sourceIP
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_sourceIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    axutil_array_list_t*  arg_sourceIP)
             {
                
                 int size = 0;
                 int i = 0;
                 axis2_bool_t non_nil_exists = AXIS2_FALSE;
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_sourceIP &&
                        arg_sourceIP == _bandwidthRequest->property_sourceIP)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                 size = axutil_array_list_size(arg_sourceIP, env);
                 
                 if (size < 0)
                 {
                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "sourceIP has less than minOccurs(0)");
                     return AXIS2_FAILURE;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != axutil_array_list_get(arg_sourceIP, env, i))
                     {
                         non_nil_exists = AXIS2_TRUE;
                         break;
                     }
                 }

                 adb_bandwidthRequest_reset_sourceIP(_bandwidthRequest, env);

                
                if(NULL == arg_sourceIP)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _bandwidthRequest->property_sourceIP = arg_sourceIP;
                        if(non_nil_exists)
                        {
                            _bandwidthRequest->is_valid_sourceIP = AXIS2_TRUE;
                        }
                        
                    
                return AXIS2_SUCCESS;
             }

            
            /**
             * Get ith element of sourceIP.
             */
            axis2_char_t* AXIS2_CALL
            adb_bandwidthRequest_get_sourceIP_at(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, int i)
            {
                axis2_char_t* ret_val;

                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                if(_bandwidthRequest->property_sourceIP == NULL)
                {
                    return (axis2_char_t*)0;
                }
                ret_val = (axis2_char_t*)axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i);
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of sourceIP.
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_sourceIP_at(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, int i,
                    const axis2_char_t* arg_sourceIP)
            {
                void *element = NULL;
                int size = 0;
                int j;
                int k;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if( _bandwidthRequest->is_valid_sourceIP &&
                    _bandwidthRequest->property_sourceIP &&
                
                    arg_sourceIP == (axis2_char_t*)axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i))
                  
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                    if(NULL == arg_sourceIP)
                    {
                        if(_bandwidthRequest->property_sourceIP != NULL)
                        {
                            size = axutil_array_list_size(_bandwidthRequest->property_sourceIP, env);
                            for(j = 0, k = 0; j < size; j ++ )
                            {
                                if(i == j) continue; 
                                if(NULL != axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of sourceIP is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }

                if(_bandwidthRequest->property_sourceIP == NULL)
                {
                    _bandwidthRequest->property_sourceIP = axutil_array_list_create(env, 10);
                }
                
                /* check whether there already exist an element */
                element = axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i);
                if(NULL != element)
                {
                  
                  
                  
                       /* This is an unknown type or a primitive. Please free this manually*/
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _bandwidthRequest->is_valid_sourceIP = AXIS2_FALSE;
                        axutil_array_list_set(_bandwidthRequest->property_sourceIP , env, i, NULL);
                        
                        return AXIS2_SUCCESS;
                    }
                
                   axutil_array_list_set(_bandwidthRequest->property_sourceIP , env, i, axutil_strdup(env, arg_sourceIP));
                  _bandwidthRequest->is_valid_sourceIP = AXIS2_TRUE;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to sourceIP.
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_add_sourceIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    const axis2_char_t* arg_sourceIP)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);

                
                    if(NULL == arg_sourceIP)
                    {
                      
                           return AXIS2_SUCCESS; 
                        
                    }
                  

                if(_bandwidthRequest->property_sourceIP == NULL)
                {
                    _bandwidthRequest->property_sourceIP = axutil_array_list_create(env, 10);
                }
                if(_bandwidthRequest->property_sourceIP == NULL)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Failed in allocatting memory for sourceIP");
                    return AXIS2_FAILURE;
                    
                }
                
                   axutil_array_list_add(_bandwidthRequest->property_sourceIP , env, axutil_strdup(env, arg_sourceIP));
                  _bandwidthRequest->is_valid_sourceIP = AXIS2_TRUE;
                return AXIS2_SUCCESS;
             }

            /**
             * Get the size of the sourceIP array.
             */
            int AXIS2_CALL
            adb_bandwidthRequest_sizeof_sourceIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
            {
                AXIS2_ENV_CHECK(env, -1);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, -1);
                if(_bandwidthRequest->property_sourceIP == NULL)
                {
                    return 0;
                }
                return axutil_array_list_size(_bandwidthRequest->property_sourceIP, env);
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_remove_sourceIP_at(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, int i)
            {
                return adb_bandwidthRequest_set_sourceIP_nil_at(_bandwidthRequest, env, i);
            }

            

           /**
            * resetter for sourceIP
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_sourceIP(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               
                  if (_bandwidthRequest->property_sourceIP != NULL)
                  {
                      count = axutil_array_list_size(_bandwidthRequest->property_sourceIP, env);
                      for(i = 0; i < count; i ++)
                      {
                         element = axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i);
                
            
                
                if(element != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, (axis2_char_t*)element);
                     element = NULL;
                }
            
                
                
                
                      }
                      axutil_array_list_free(_bandwidthRequest->property_sourceIP, env);
                  }
                _bandwidthRequest->is_valid_sourceIP = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether sourceIP is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_sourceIP_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_sourceIP;
           }

           /**
            * Set sourceIP to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_sourceIP_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_sourceIP(_bandwidthRequest, env);
           }

           
           /**
            * Check whether sourceIP is nill at i
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_sourceIP_nil_at(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env, int i)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return (_bandwidthRequest->is_valid_sourceIP == AXIS2_FALSE ||
                        NULL == _bandwidthRequest->property_sourceIP || 
                        NULL == axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i));
           }

           /**
            * Set sourceIP to nill at i
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_sourceIP_nil_at(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env, int i)
           {
                void *element = NULL;
                int size = 0;
                int j;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                int k = 0;

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);

                if(_bandwidthRequest->property_sourceIP == NULL ||
                            _bandwidthRequest->is_valid_sourceIP == AXIS2_FALSE)
                {
                    
                    non_nil_exists = AXIS2_FALSE;
                }
                else
                {
                    size = axutil_array_list_size(_bandwidthRequest->property_sourceIP, env);
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of sourceIP is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }
 
                if(_bandwidthRequest->property_sourceIP == NULL)
                {
                    _bandwidthRequest->is_valid_sourceIP = AXIS2_FALSE;
                    
                    return AXIS2_SUCCESS;
                }

                /* check whether there already exist an element */
                element = axutil_array_list_get(_bandwidthRequest->property_sourceIP, env, i);
                if(NULL != element)
                {
                  
                  
                  
                       /* This is an unknown type or a primitive. Please free this manually*/
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _bandwidthRequest->is_valid_sourceIP = AXIS2_FALSE;
                        axutil_array_list_set(_bandwidthRequest->property_sourceIP , env, i, NULL);
                        return AXIS2_SUCCESS;
                    }
                

                
                axutil_array_list_set(_bandwidthRequest->property_sourceIP , env, i, NULL);
                
                return AXIS2_SUCCESS;

           }

           

            /**
             * getter for sourceSipAddress.
             */
            axis2_char_t* AXIS2_CALL
            adb_bandwidthRequest_get_sourceSipAddress(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                return _bandwidthRequest->property_sourceSipAddress;
             }

            /**
             * setter for sourceSipAddress
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_sourceSipAddress(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    const axis2_char_t*  arg_sourceSipAddress)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_sourceSipAddress &&
                        arg_sourceSipAddress == _bandwidthRequest->property_sourceSipAddress)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_bandwidthRequest_reset_sourceSipAddress(_bandwidthRequest, env);

                
                if(NULL == arg_sourceSipAddress)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _bandwidthRequest->property_sourceSipAddress = (axis2_char_t *)axutil_strdup(env, arg_sourceSipAddress);
                        if(NULL == _bandwidthRequest->property_sourceSipAddress)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Error allocating memeory for sourceSipAddress");
                            return AXIS2_FAILURE;
                        }
                        _bandwidthRequest->is_valid_sourceSipAddress = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for sourceSipAddress
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_sourceSipAddress(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               
            
                
                if(_bandwidthRequest->property_sourceSipAddress != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, _bandwidthRequest->property_sourceSipAddress);
                     _bandwidthRequest->property_sourceSipAddress = NULL;
                }
            
                
                
                _bandwidthRequest->is_valid_sourceSipAddress = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether sourceSipAddress is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_sourceSipAddress_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_sourceSipAddress;
           }

           /**
            * Set sourceSipAddress to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_sourceSipAddress_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_sourceSipAddress(_bandwidthRequest, env);
           }

           

            /**
             * getter for start.
             */
            axutil_date_time_t* AXIS2_CALL
            adb_bandwidthRequest_get_start(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, NULL);
                  

                return _bandwidthRequest->property_start;
             }

            /**
             * setter for start
             */
            axis2_status_t AXIS2_CALL
            adb_bandwidthRequest_set_start(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env,
                    axutil_date_time_t*  arg_start)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
                
                if(_bandwidthRequest->is_valid_start &&
                        arg_start == _bandwidthRequest->property_start)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_bandwidthRequest_reset_start(_bandwidthRequest, env);

                
                if(NULL == arg_start)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _bandwidthRequest->property_start = arg_start;
                        _bandwidthRequest->is_valid_start = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for start
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_reset_start(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_FAILURE);
               

               
            
                
                if(_bandwidthRequest->property_start != NULL)
                {
                   
                   
                      axutil_date_time_free(_bandwidthRequest->property_start, env);
                     _bandwidthRequest->property_start = NULL;
                }
            
                
                
                _bandwidthRequest->is_valid_start = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether start is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_bandwidthRequest_is_start_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _bandwidthRequest, AXIS2_TRUE);
               
               return !_bandwidthRequest->is_valid_start;
           }

           /**
            * Set start to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_bandwidthRequest_set_start_nil(
                   adb_bandwidthRequest_t* _bandwidthRequest,
                   const axutil_env_t *env)
           {
               return adb_bandwidthRequest_reset_start(_bandwidthRequest, env);
           }

           

