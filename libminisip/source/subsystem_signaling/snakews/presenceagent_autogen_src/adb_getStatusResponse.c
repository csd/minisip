

        /**
         * adb_getStatusResponse.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_getStatusResponse.h"
        
                /*
                 * This type was generated from the piece of schema that had
                 * name = getStatusResponse
                 * Namespace URI = http://esb.presenceagent.services.hdviper.psnc.pl/
                 * Namespace Prefix = ns1
                 */
           


        struct adb_getStatusResponse
        {
            adb_userStatus_t* property_userStatus;

                
                axis2_bool_t is_valid_userStatus;


            
        };


       /************************* Private Function prototypes ********************************/
        

                axis2_status_t AXIS2_CALL
                adb_getStatusResponse_set_userStatus_nil(
                        adb_getStatusResponse_t* _getStatusResponse,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_getStatusResponse_t* AXIS2_CALL
        adb_getStatusResponse_create(
            const axutil_env_t *env)
        {
            adb_getStatusResponse_t *_getStatusResponse = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _getStatusResponse = (adb_getStatusResponse_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_getStatusResponse_t));

            if(NULL == _getStatusResponse)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_getStatusResponse, 0, sizeof(adb_getStatusResponse_t));

            _getStatusResponse->property_userStatus  = NULL;
                  _getStatusResponse->is_valid_userStatus  = AXIS2_FALSE;
            

            return _getStatusResponse;
        }

        axis2_status_t AXIS2_CALL
        adb_getStatusResponse_free (
                adb_getStatusResponse_t* _getStatusResponse,
                const axutil_env_t *env)
        {
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _getStatusResponse, AXIS2_FAILURE);

            adb_getStatusResponse_reset_userStatus(_getStatusResponse, env);
            

            if(_getStatusResponse)
            {
                AXIS2_FREE(env->allocator, _getStatusResponse);
                _getStatusResponse = NULL;
            }
            return AXIS2_SUCCESS;
        }


        

        axis2_status_t AXIS2_CALL
        adb_getStatusResponse_deserialize(
                adb_getStatusResponse_t* _getStatusResponse,
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
            AXIS2_PARAM_CHECK(env->error, _getStatusResponse, AXIS2_FAILURE);

            
              
              while(parent && axiom_node_get_node_type(parent, env) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, env);
              }
              if (NULL == parent)
              {
                /* This should be checked before everything */
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, 
                            "Failed in building adb object for getStatusResponse : "
                            "NULL elemenet can not be passed to deserialize");
                return AXIS2_FAILURE;
              }
              
                      
                      first_node = axiom_node_get_first_child(parent, env);
                      
                    

                     
                     /*
                      * building userStatus element
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
                                   
                                 element_qname = axutil_qname_create(env, "userStatus", NULL, NULL);
                                 

                           if (adb_userStatus_is_particle() ||  
                                (current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("userStatus", axiom_element_get_localname(current_element, env)))))
                           {
                              if( current_node   && current_element && (axutil_qname_equals(element_qname, env, qname) || !axutil_strcmp("userStatus", axiom_element_get_localname(current_element, env))))
                              {
                                is_early_node_valid = AXIS2_TRUE;
                              }
                              
                                 
                                      element = (void*)adb_userStatus_create(env);

                                      status =  adb_userStatus_deserialize((adb_userStatus_t*)element,
                                                                            env, &current_node, &is_early_node_valid, AXIS2_FALSE);
                                      if(AXIS2_FAILURE == status)
                                      {
                                          AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in building adb object for element userStatus");
                                      }
                                      else
                                      {
                                          status = adb_getStatusResponse_set_userStatus(_getStatusResponse, env,
                                                                   (adb_userStatus_t*)element);
                                      }
                                    
                                 if(AXIS2_FAILURE ==  status)
                                 {
                                     AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "failed in setting the value for userStatus ");
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
          adb_getStatusResponse_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_getStatusResponse_declare_parent_namespaces(
                    adb_getStatusResponse_t* _getStatusResponse,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* AXIS2_CALL
        adb_getStatusResponse_serialize(
                adb_getStatusResponse_t* _getStatusResponse,
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
            
                    axis2_char_t text_value_1[64];
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

            

            AXIS2_ENV_CHECK(env, NULL);
            AXIS2_PARAM_CHECK(env->error, _getStatusResponse, NULL);
            
            
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
                      

                   if (!_getStatusResponse->is_valid_userStatus)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("userStatus"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(env->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("userStatus")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     
                     /*
                      * parsing userStatus element
                      */

                    
                    
                            sprintf(start_input_str, "<%s%suserStatus",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                        start_input_str_len = axutil_strlen(start_input_str);
                        sprintf(end_input_str, "</%s%suserStatus>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                        end_input_str_len = axutil_strlen(end_input_str);
                     
                            if(!adb_userStatus_is_particle())
                            {
                                axutil_stream_write(stream, env, start_input_str, start_input_str_len);
                            }
                            
                            adb_userStatus_serialize(_getStatusResponse->property_userStatus, 
                                                                                 env, current_node, parent_element,
                                                                                 adb_userStatus_is_particle() || AXIS2_FALSE, namespaces, next_ns_index);
                            
                            if(!adb_userStatus_is_particle())
                            {
                                axutil_stream_write(stream, env, end_input_str, end_input_str_len);
                            }
                            
                     
                     AXIS2_FREE(env->allocator,start_input_str);
                     AXIS2_FREE(env->allocator,end_input_str);
                 } 

                 

            return parent;
        }


        

            /**
             * getter for userStatus.
             */
            adb_userStatus_t* AXIS2_CALL
            adb_getStatusResponse_get_userStatus(
                    adb_getStatusResponse_t* _getStatusResponse,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _getStatusResponse, NULL);
                  

                return _getStatusResponse->property_userStatus;
             }

            /**
             * setter for userStatus
             */
            axis2_status_t AXIS2_CALL
            adb_getStatusResponse_set_userStatus(
                    adb_getStatusResponse_t* _getStatusResponse,
                    const axutil_env_t *env,
                    adb_userStatus_t*  arg_userStatus)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _getStatusResponse, AXIS2_FAILURE);
                
                if(_getStatusResponse->is_valid_userStatus &&
                        arg_userStatus == _getStatusResponse->property_userStatus)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                adb_getStatusResponse_reset_userStatus(_getStatusResponse, env);

                
                if(NULL == arg_userStatus)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _getStatusResponse->property_userStatus = arg_userStatus;
                        _getStatusResponse->is_valid_userStatus = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for userStatus
            */
           axis2_status_t AXIS2_CALL
           adb_getStatusResponse_reset_userStatus(
                   adb_getStatusResponse_t* _getStatusResponse,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _getStatusResponse, AXIS2_FAILURE);
               

               
            
                
                if(_getStatusResponse->property_userStatus != NULL)
                {
                   
                   
                        adb_userStatus_free(_getStatusResponse->property_userStatus, env);
                     _getStatusResponse->property_userStatus = NULL;
                }
            
                
                
                _getStatusResponse->is_valid_userStatus = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether userStatus is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_getStatusResponse_is_userStatus_nil(
                   adb_getStatusResponse_t* _getStatusResponse,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _getStatusResponse, AXIS2_TRUE);
               
               return !_getStatusResponse->is_valid_userStatus;
           }

           /**
            * Set userStatus to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_getStatusResponse_set_userStatus_nil(
                   adb_getStatusResponse_t* _getStatusResponse,
                   const axutil_env_t *env)
           {
               return adb_getStatusResponse_reset_userStatus(_getStatusResponse, env);
           }

           

