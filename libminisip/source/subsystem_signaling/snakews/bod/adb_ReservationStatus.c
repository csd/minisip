

        /**
         * adb_ReservationStatus.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_ReservationStatus.h"
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = ReservationStatus
                 * Namespace URI = http://esb.bod.services.hdviper.i2cat.net/
                 * Namespace Prefix = ns1
                 */
           


        struct adb_ReservationStatus
        {
            axis2_char_t* property_reservationId;

                
                axis2_bool_t is_valid_reservationId;


            adb_reservationState_t* property_state;

                
                axis2_bool_t is_valid_state;


            
        };


       /************************* Private Function prototypes ********************************/
        

                axis2_status_t AXIS2_CALL
                adb_ReservationStatus_set_reservationId_nil(
                        adb_ReservationStatus_t* _ReservationStatus,
                        const axutil_env_t *env);
            

                axis2_status_t AXIS2_CALL
                adb_ReservationStatus_set_state_nil(
                        adb_ReservationStatus_t* _ReservationStatus,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_ReservationStatus_t* AXIS2_CALL
        adb_ReservationStatus_create(
            const axutil_env_t *env)
        {
            adb_ReservationStatus_t *_ReservationStatus = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _ReservationStatus = (adb_ReservationStatus_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_ReservationStatus_t));

            if(NULL == _ReservationStatus)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_ReservationStatus, 0, sizeof(adb_ReservationStatus_t));

            _ReservationStatus->property_reservationId  = NULL;
                  _ReservationStatus->is_valid_reservationId  = AXIS2_FALSE;
            _ReservationStatus->property_state  = NULL;
                  _ReservationStatus->is_valid_state  = AXIS2_FALSE;
            

            return _ReservationStatus;
        }

        axis2_status_t AXIS2_CALL
        adb_ReservationStatus_free (
                adb_ReservationStatus_t* _ReservationStatus,
                const axutil_env_t *env)
        {
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_FAILURE);

            adb_ReservationStatus_reset_reservationId(_ReservationStatus, env);
            adb_ReservationStatus_reset_state(_ReservationStatus, env);
            

            if(_ReservationStatus)
            {
                AXIS2_FREE(env->allocator, _ReservationStatus);
                _ReservationStatus = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_ReservationStatus_deserialize(
                adb_ReservationStatus_t* _ReservationStatus,
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
            AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for ReservationStatus : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, env);
                      
                    

                     
                     /*
                      * building reservationId element
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
                                   
                                 element_qname = axutil_qname_create(env, "reservationId", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("reservationId", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("reservationId", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                            status = adb_ReservationStatus_set_reservationId(_ReservationStatus, env,
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
                                                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element reservationId");
                                                status = AXIS2_FAILURE;
                                            }
                                            else
                                            {
                                                /* after all, we found this is a empty string */
                                                status = adb_ReservationStatus_set_reservationId(_ReservationStatus, env,
                                                                   "");
                                            }
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for reservationId ");
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
                      * building state element
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
                                 
                                 element_qname = axutil_qname_create(env, "state", NULL, NULL);
                                 

                           if (adb_reservationState_is_particle() ||  
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("state", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("state", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      element = (void*)adb_reservationState_create(env);

                                      status =  adb_reservationState_deserialize((adb_reservationState_t*)element,
                                                                            env, &current_node, &is_early_node_valid, AXIS2_FALSE);
                                      if(AXIS2_FAILURE == status)
                                      {
                                          AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building adb object for element state");
                                      }
                                      else
                                      {
                                          status = adb_ReservationStatus_set_state(_ReservationStatus, env,
                                                                   (adb_reservationState_t*)element);
                                      }
                                    
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for state ");
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
          adb_ReservationStatus_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_ReservationStatus_declare_parent_namespaces(
                    adb_ReservationStatus_t* _ReservationStatus,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_ReservationStatus_serialize(
                adb_ReservationStatus_t* _ReservationStatus,
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
            
                    axis2_char_t *text_value_1;
                    axis2_char_t *text_value_1_temp;
                    
                    axis2_char_t text_value_2[64];
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _ReservationStatus, NULL);
            
            
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
                      

                   if (!_ReservationStatus->is_valid_reservationId)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("reservationId"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("reservationId")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing reservationId element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sreservationId>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sreservationId>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                           text_value_1 = _ReservationStatus->property_reservationId;
                           
                           axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                           
                            
                           text_value_1_temp = axutil_xml_quote_string(env, text_value_1, AXIS2_TRUE);
                           if (text_value_1_temp)
                           {
                               axutil_stream_write(stream, env, text_value_1_temp, axutil_strlen(text_value_1_temp));
                               AXIS2_FREE(env->allocator, text_value_1_temp);
                           }
                           else
                           {
                               axutil_stream_write(stream, env, text_value_1, axutil_strlen(text_value_1));
                           }
                           
                           axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                           
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 
                       p_prefix = NULL;
                      

                   if (!_ReservationStatus->is_valid_state)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("state"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("state")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing state element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%sstate",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%sstate>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                     
                            if(!adb_reservationState_is_particle())
                            {
                                axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                            }
                            
                            adb_reservationState_serialize(_ReservationStatus->property_state, 
                                                                                 env, current_node, parent_element,
                                                                                 adb_reservationState_is_particle() || AXIS2_FALSE, namespaces, next_ns_index);
                            
                            if(!adb_reservationState_is_particle())
                            {
                                axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                            }
                            
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 

            return parent;
        }


        

            /**
             * getter for reservationId.
             */
            axis2_char_t* AXIS2_CALL
            adb_ReservationStatus_get_reservationId(
                    adb_ReservationStatus_t* _ReservationStatus,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _ReservationStatus, NULL);
                  

                return _ReservationStatus->property_reservationId;
             }

            /**
             * setter for reservationId
             */
            axis2_status_t AXIS2_CALL
            adb_ReservationStatus_set_reservationId(
                    adb_ReservationStatus_t* _ReservationStatus,
                    const axutil_env_t *env,
                    const axis2_char_t*  arg_reservationId)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_FAILURE);
                
                if(_ReservationStatus->is_valid_reservationId &&
                        arg_reservationId == _ReservationStatus->property_reservationId)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_ReservationStatus_reset_reservationId(_ReservationStatus, env);

                
                if(NULL == arg_reservationId)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _ReservationStatus->property_reservationId = (axis2_char_t *)axutil_strdup(env, arg_reservationId);
                        if(NULL == _ReservationStatus->property_reservationId)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Error allocating memeory for reservationId");
                            return AXIS2_FAILURE;
                        }
                        _ReservationStatus->is_valid_reservationId = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for reservationId
            */
           axis2_status_t AXIS2_CALL
           adb_ReservationStatus_reset_reservationId(
                   adb_ReservationStatus_t* _ReservationStatus,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_FAILURE);
               

               
            
                
                if(_ReservationStatus->property_reservationId != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, _ReservationStatus->property_reservationId);
                     _ReservationStatus->property_reservationId = NULL;
                }
            
                
                
                _ReservationStatus->is_valid_reservationId = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether reservationId is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_ReservationStatus_is_reservationId_nil(
                   adb_ReservationStatus_t* _ReservationStatus,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_TRUE);
               
               return !_ReservationStatus->is_valid_reservationId;
           }

           /**
            * Set reservationId to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_ReservationStatus_set_reservationId_nil(
                   adb_ReservationStatus_t* _ReservationStatus,
                   const axutil_env_t *env)
           {
               return adb_ReservationStatus_reset_reservationId(_ReservationStatus, env);
           }

           

            /**
             * getter for state.
             */
            adb_reservationState_t* AXIS2_CALL
            adb_ReservationStatus_get_state(
                    adb_ReservationStatus_t* _ReservationStatus,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _ReservationStatus, NULL);
                  

                return _ReservationStatus->property_state;
             }

            /**
             * setter for state
             */
            axis2_status_t AXIS2_CALL
            adb_ReservationStatus_set_state(
                    adb_ReservationStatus_t* _ReservationStatus,
                    const axutil_env_t *env,
                    adb_reservationState_t*  arg_state)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_FAILURE);
                
                if(_ReservationStatus->is_valid_state &&
                        arg_state == _ReservationStatus->property_state)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_ReservationStatus_reset_state(_ReservationStatus, env);

                
                if(NULL == arg_state)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _ReservationStatus->property_state = arg_state;
                        _ReservationStatus->is_valid_state = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for state
            */
           axis2_status_t AXIS2_CALL
           adb_ReservationStatus_reset_state(
                   adb_ReservationStatus_t* _ReservationStatus,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_FAILURE);
               

               
            
                
                if(_ReservationStatus->property_state != NULL)
                {
                   
                   
                        adb_reservationState_free(_ReservationStatus->property_state, env);
                     _ReservationStatus->property_state = NULL;
                }
            
                
                
                _ReservationStatus->is_valid_state = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether state is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_ReservationStatus_is_state_nil(
                   adb_ReservationStatus_t* _ReservationStatus,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _ReservationStatus, AXIS2_TRUE);
               
               return !_ReservationStatus->is_valid_state;
           }

           /**
            * Set state to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_ReservationStatus_set_state_nil(
                   adb_ReservationStatus_t* _ReservationStatus,
                   const axutil_env_t *env)
           {
               return adb_ReservationStatus_reset_state(_ReservationStatus, env);
           }

           

