

        /**
         * adb_setContactList.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_setContactList.h"
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = setContactList
                 * Namespace URI = http://esb.presenceagent.services.hdviper.psnc.pl/
                 * Namespace Prefix = ns1
                 */
           


        struct adb_setContactList
        {
            axis2_char_t* property_sessionId;

                
                axis2_bool_t is_valid_sessionId;


            axutil_array_list_t* property_contactAddress;

                
                axis2_bool_t is_valid_contactAddress;


            
        };


       /************************* Private Function prototypes ********************************/
        

                axis2_status_t AXIS2_CALL
                adb_setContactList_set_sessionId_nil(
                        adb_setContactList_t* _setContactList,
                        const axutil_env_t *env);
            
                 axis2_status_t AXIS2_CALL
                 adb_setContactList_set_contactAddress_nil_at(
                        adb_setContactList_t* _setContactList, 
                        const axutil_env_t *env, int i);
                

                axis2_status_t AXIS2_CALL
                adb_setContactList_set_contactAddress_nil(
                        adb_setContactList_t* _setContactList,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_setContactList_t* AXIS2_CALL
        adb_setContactList_create(
            const axutil_env_t *env)
        {
            adb_setContactList_t *_setContactList = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _setContactList = (adb_setContactList_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_setContactList_t));

            if(NULL == _setContactList)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_setContactList, 0, sizeof(adb_setContactList_t));

            _setContactList->property_sessionId  = NULL;
                  _setContactList->is_valid_sessionId  = AXIS2_FALSE;
            _setContactList->property_contactAddress  = NULL;
                  _setContactList->is_valid_contactAddress  = AXIS2_FALSE;
            

            return _setContactList;
        }

        axis2_status_t AXIS2_CALL
        adb_setContactList_free (
                adb_setContactList_t* _setContactList,
                const axutil_env_t *env)
        {
            
                int i = 0;
                int count = 0;
                void *element = NULL;
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);

            adb_setContactList_reset_sessionId(_setContactList, env);
            adb_setContactList_reset_contactAddress(_setContactList, env);
            

            if(_setContactList)
            {
                AXIS2_FREE(env->allocator, _setContactList);
                _setContactList = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_setContactList_deserialize(
                adb_setContactList_t* _setContactList,
                const axutil_env_t *env,
                axiom_node_t **dp_parent,
                axis2_bool_t *dp_is_early_node_valid,
                axis2_bool_t dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          axis2_status_t status = AXIS2_SUCCESS;
           
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
            AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for setContactList : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, env);
                      
                    

                     
                     /*
                      * building sessionId element
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
                                   
                                 element_qname = axutil_qname_create(env, "sessionId", NULL, NULL);
                                 

                           if ( 
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("sessionId", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("sessionId", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      text_value = axiom_element_get_text(current_element, env, current_node);
                                      if(text_value != NULL)
                                      {
                                            status = adb_setContactList_set_sessionId(_setContactList, env,
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
                                                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element sessionId");
                                                status = AXIS2_FAILURE;
                                            }
                                            else
                                            {
                                                /* after all, we found this is a empty string */
                                                status = adb_setContactList_set_sessionId(_setContactList, env,
                                                                   "");
                                            }
                                      }
                                      
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for sessionId ");
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
                     * building contactAddress array
                     */
                       arr_list = axutil_array_list_create(env, 10);
                   

                     
                     /*
                      * building contactAddress element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(env, "contactAddress", NULL, NULL);
                                  
                               
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

                                  if (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("contactAddress", axiom_element_get_localname(current_element, env)))
                                  {
                                  
                                      is_early_node_valid = AXIS2_TRUE;
                                      
                                     
                                          text_value = axiom_element_get_text(current_element, env, current_node);
                                          if(text_value != NULL)
                                          {
                                              axutil_array_list_add_at(arr_list, env, i, axutil_strdup(env, text_value));
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
                                                  AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "NULL value is set to a non nillable element contactAddress");
                                                  status = AXIS2_FAILURE;
                                              }
                                              else
                                              {
                                                  /* after all, we found this is a empty string */
                                                  axutil_array_list_add_at(arr_list, env, i, axutil_strdup(env, ""));
                                              }
                                          }
                                          
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for contactAddress ");
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
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "contactAddress (@minOccurs = '0') only have %d elements", i);
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
                                    status = adb_setContactList_set_contactAddress(_setContactList, env,
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
          adb_setContactList_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_setContactList_declare_parent_namespaces(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_setContactList_serialize(
                adb_setContactList_t* _setContactList,
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
             
                    axis2_char_t *text_value_1;
                    axis2_char_t *text_value_1_temp;
                    
                    axis2_char_t *text_value_2;
                    axis2_char_t *text_value_2_temp;
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _setContactList, NULL);
            
            
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
                      

                   if (!_setContactList->is_valid_sessionId)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("sessionId"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("sessionId")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing sessionId element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%ssessionId>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%ssessionId>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                    
                           text_value_1 = _setContactList->property_sessionId;
                           
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
                      

                   if (!_setContactList->is_valid_contactAddress)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("contactAddress"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("contactAddress")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing contactAddress array
                      */
                     if (_setContactList->property_contactAddress != NULL)
                     {
                        
                            sprintf(start_input_str, "<%s%scontactAddress>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%scontactAddress>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = axutil_array_list_size(_setContactList->property_contactAddress, env);
                         for(i = 0; i < count; i ++)
                         {
                            element = axutil_array_list_get(_setContactList->property_contactAddress, env, i);

                            if(NULL == element) 
                            {
                                continue;
                            }
                    
                     
                     /*
                      * parsing contactAddress element
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

                 

            return parent;
        }


        

            /**
             * getter for sessionId.
             */
            axis2_char_t* AXIS2_CALL
            adb_setContactList_get_sessionId(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _setContactList, NULL);
                  

                return _setContactList->property_sessionId;
             }

            /**
             * setter for sessionId
             */
            axis2_status_t AXIS2_CALL
            adb_setContactList_set_sessionId(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env,
                    const axis2_char_t*  arg_sessionId)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);
                
                if(_setContactList->is_valid_sessionId &&
                        arg_sessionId == _setContactList->property_sessionId)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_setContactList_reset_sessionId(_setContactList, env);

                
                if(NULL == arg_sessionId)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _setContactList->property_sessionId = (axis2_char_t *)axutil_strdup(env, arg_sessionId);
                        if(NULL == _setContactList->property_sessionId)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Error allocating memeory for sessionId");
                            return AXIS2_FAILURE;
                        }
                        _setContactList->is_valid_sessionId = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for sessionId
            */
           axis2_status_t AXIS2_CALL
           adb_setContactList_reset_sessionId(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);
               

               
            
                
                if(_setContactList->property_sessionId != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, _setContactList->property_sessionId);
                     _setContactList->property_sessionId = NULL;
                }
            
                
                
                _setContactList->is_valid_sessionId = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether sessionId is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_setContactList_is_sessionId_nil(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_TRUE);
               
               return !_setContactList->is_valid_sessionId;
           }

           /**
            * Set sessionId to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_setContactList_set_sessionId_nil(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env)
           {
               return adb_setContactList_reset_sessionId(_setContactList, env);
           }

           

            /**
             * getter for contactAddress.
             */
            axutil_array_list_t* AXIS2_CALL
            adb_setContactList_get_contactAddress(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _setContactList, NULL);
                  

                return _setContactList->property_contactAddress;
             }

            /**
             * setter for contactAddress
             */
            axis2_status_t AXIS2_CALL
            adb_setContactList_set_contactAddress(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env,
                    axutil_array_list_t*  arg_contactAddress)
             {
                
                 int size = 0;
                 int i = 0;
                 axis2_bool_t non_nil_exists = AXIS2_FALSE;
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);
                
                if(_setContactList->is_valid_contactAddress &&
                        arg_contactAddress == _setContactList->property_contactAddress)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                 size = axutil_array_list_size(arg_contactAddress, env);
                 
                 if (size < 0)
                 {
                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "contactAddress has less than minOccurs(0)");
                     return AXIS2_FAILURE;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != axutil_array_list_get(arg_contactAddress, env, i))
                     {
                         non_nil_exists = AXIS2_TRUE;
                         break;
                     }
                 }

                 adb_setContactList_reset_contactAddress(_setContactList, env);

                
                if(NULL == arg_contactAddress)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _setContactList->property_contactAddress = arg_contactAddress;
                        if(non_nil_exists)
                        {
                            _setContactList->is_valid_contactAddress = AXIS2_TRUE;
                        }
                        
                    
                return AXIS2_SUCCESS;
             }

            
            /**
             * Get ith element of contactAddress.
             */
            axis2_char_t* AXIS2_CALL
            adb_setContactList_get_contactAddress_at(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env, int i)
            {
                axis2_char_t* ret_val;

                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _setContactList, NULL);
                  

                if(_setContactList->property_contactAddress == NULL)
                {
                    return (axis2_char_t*)0;
                }
                ret_val = (axis2_char_t*)axutil_array_list_get(_setContactList->property_contactAddress, env, i);
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of contactAddress.
             */
            axis2_status_t AXIS2_CALL
            adb_setContactList_set_contactAddress_at(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env, int i,
                    const axis2_char_t* arg_contactAddress)
            {
                void *element = NULL;
                int size = 0;
                int j;
                int k;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);
                
                if( _setContactList->is_valid_contactAddress &&
                    _setContactList->property_contactAddress &&
                
                    arg_contactAddress == (axis2_char_t*)axutil_array_list_get(_setContactList->property_contactAddress, env, i))
                  
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                    if(NULL == arg_contactAddress)
                    {
                        if(_setContactList->property_contactAddress != NULL)
                        {
                            size = axutil_array_list_size(_setContactList->property_contactAddress, env);
                            for(j = 0, k = 0; j < size; j ++ )
                            {
                                if(i == j) continue; 
                                if(NULL != axutil_array_list_get(_setContactList->property_contactAddress, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of contactAddress is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }

                if(_setContactList->property_contactAddress == NULL)
                {
                    _setContactList->property_contactAddress = axutil_array_list_create(env, 10);
                }
                
                /* check whether there already exist an element */
                element = axutil_array_list_get(_setContactList->property_contactAddress, env, i);
                if(NULL != element)
                {
                  
                  
                  
                       /* This is an unknown type or a primitive. Please free this manually*/
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _setContactList->is_valid_contactAddress = AXIS2_FALSE;
                        axutil_array_list_set(_setContactList->property_contactAddress , env, i, NULL);
                        
                        return AXIS2_SUCCESS;
                    }
                
                   axutil_array_list_set(_setContactList->property_contactAddress , env, i, axutil_strdup(env, arg_contactAddress));
                  _setContactList->is_valid_contactAddress = AXIS2_TRUE;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to contactAddress.
             */
            axis2_status_t AXIS2_CALL
            adb_setContactList_add_contactAddress(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env,
                    const axis2_char_t* arg_contactAddress)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);

                
                    if(NULL == arg_contactAddress)
                    {
                      
                           return AXIS2_SUCCESS; 
                        
                    }
                  

                if(_setContactList->property_contactAddress == NULL)
                {
                    _setContactList->property_contactAddress = axutil_array_list_create(env, 10);
                }
                if(_setContactList->property_contactAddress == NULL)
                {
                    AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Failed in allocatting memory for contactAddress");
                    return AXIS2_FAILURE;
                    
                }
                
                   axutil_array_list_add(_setContactList->property_contactAddress , env, axutil_strdup(env, arg_contactAddress));
                  _setContactList->is_valid_contactAddress = AXIS2_TRUE;
                return AXIS2_SUCCESS;
             }

            /**
             * Get the size of the contactAddress array.
             */
            int AXIS2_CALL
            adb_setContactList_sizeof_contactAddress(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env)
            {
                AXIS2_ENV_CHECK(env, -1);
                AXIS2_PARAM_CHECK(env->error, _setContactList, -1);
                if(_setContactList->property_contactAddress == NULL)
                {
                    return 0;
                }
                return axutil_array_list_size(_setContactList->property_contactAddress, env);
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            axis2_status_t AXIS2_CALL
            adb_setContactList_remove_contactAddress_at(
                    adb_setContactList_t* _setContactList,
                    const axutil_env_t *env, int i)
            {
                return adb_setContactList_set_contactAddress_nil_at(_setContactList, env, i);
            }

            

           /**
            * resetter for contactAddress
            */
           axis2_status_t AXIS2_CALL
           adb_setContactList_reset_contactAddress(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);
               

               
                  if (_setContactList->property_contactAddress != NULL)
                  {
                      count = axutil_array_list_size(_setContactList->property_contactAddress, env);
                      for(i = 0; i < count; i ++)
                      {
                         element = axutil_array_list_get(_setContactList->property_contactAddress, env, i);
                
            
                
                if(element != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, (axis2_char_t*)element);
                     element = NULL;
                }
            
                
                
                
                      }
                      axutil_array_list_free(_setContactList->property_contactAddress, env);
                  }
                _setContactList->is_valid_contactAddress = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether contactAddress is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_setContactList_is_contactAddress_nil(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_TRUE);
               
               return !_setContactList->is_valid_contactAddress;
           }

           /**
            * Set contactAddress to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_setContactList_set_contactAddress_nil(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env)
           {
               return adb_setContactList_reset_contactAddress(_setContactList, env);
           }

           
           /**
            * Check whether contactAddress is nill at i
            */
           axis2_bool_t AXIS2_CALL
           adb_setContactList_is_contactAddress_nil_at(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env, int i)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_TRUE);
               
               return (_setContactList->is_valid_contactAddress == AXIS2_FALSE ||
                        NULL == _setContactList->property_contactAddress || 
                        NULL == axutil_array_list_get(_setContactList->property_contactAddress, env, i));
           }

           /**
            * Set contactAddress to nill at i
            */
           axis2_status_t AXIS2_CALL
           adb_setContactList_set_contactAddress_nil_at(
                   adb_setContactList_t* _setContactList,
                   const axutil_env_t *env, int i)
           {
                void *element = NULL;
                int size = 0;
                int j;
                axis2_bool_t non_nil_exists = AXIS2_FALSE;

                int k = 0;

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _setContactList, AXIS2_FAILURE);

                if(_setContactList->property_contactAddress == NULL ||
                            _setContactList->is_valid_contactAddress == AXIS2_FALSE)
                {
                    
                    non_nil_exists = AXIS2_FALSE;
                }
                else
                {
                    size = axutil_array_list_size(_setContactList->property_contactAddress, env);
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != axutil_array_list_get(_setContactList->property_contactAddress, env, i))
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
                       AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Size of the array of contactAddress is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }
 
                if(_setContactList->property_contactAddress == NULL)
                {
                    _setContactList->is_valid_contactAddress = AXIS2_FALSE;
                    
                    return AXIS2_SUCCESS;
                }

                /* check whether there already exist an element */
                element = axutil_array_list_get(_setContactList->property_contactAddress, env, i);
                if(NULL != element)
                {
                  
                  
                  
                       /* This is an unknown type or a primitive. Please free this manually*/
                     
                }

                
                    if(!non_nil_exists)
                    {
                        
                        _setContactList->is_valid_contactAddress = AXIS2_FALSE;
                        axutil_array_list_set(_setContactList->property_contactAddress , env, i, NULL);
                        return AXIS2_SUCCESS;
                    }
                

                
                axutil_array_list_set(_setContactList->property_contactAddress , env, i, NULL);
                
                return AXIS2_SUCCESS;

           }

           

