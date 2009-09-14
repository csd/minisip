

        /**
         * adb_statusType.c
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */

        #include "adb_statusType.h"
        
               /*
                * implmentation of the statusType|http://esb.presenceagent.services.hdviper.psnc.pl/ element
                */
           


        struct adb_statusType
        {
            
                axutil_qname_t* qname;
            axis2_char_t* property_statusType;

                
                axis2_bool_t is_valid_statusType;


            
        };


       /************************* Private Function prototypes ********************************/
        

                axis2_status_t AXIS2_CALL
                adb_statusType_set_statusType_nil(
                        adb_statusType_t* _statusType,
                        const axutil_env_t *env);
            


       /************************* Function Implmentations ********************************/
        adb_statusType_t* AXIS2_CALL
        adb_statusType_create(
            const axutil_env_t *env)
        {
            adb_statusType_t *_statusType = NULL;
            
                axutil_qname_t* qname = NULL;
            
            AXIS2_ENV_CHECK(env, NULL);

            _statusType = (adb_statusType_t *) AXIS2_MALLOC(env->
                allocator, sizeof(adb_statusType_t));

            if(NULL == _statusType)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
                return NULL;
            }

            memset(_statusType, 0, sizeof(adb_statusType_t));

            _statusType->property_statusType  = NULL;
                  _statusType->is_valid_statusType  = AXIS2_FALSE;
            
                  qname =  axutil_qname_create (env,
                        "statusType",
                        "http://esb.presenceagent.services.hdviper.psnc.pl/",
                        NULL);
                _statusType->qname = qname;
            

            return _statusType;
        }

        axis2_status_t AXIS2_CALL
        adb_statusType_free (
                adb_statusType_t* _statusType,
                const axutil_env_t *env)
        {
            

            AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
            AXIS2_PARAM_CHECK(env->error, _statusType, AXIS2_FAILURE);

            adb_statusType_reset_statusType(_statusType, env);
            
              if(_statusType->qname)
              {
                  axutil_qname_free (_statusType->qname, env);
                  _statusType->qname = NULL;
              }
            

            if(_statusType)
            {
                AXIS2_FREE(env->allocator, _statusType);
                _statusType = NULL;
            }
            return AXIS2_SUCCESS;
        }


        
            axis2_status_t AXIS2_CALL
            adb_statusType_deserialize_from_string(
                            adb_statusType_t* _statusType,
                                            const axutil_env_t *env,
                                            axis2_char_t *node_value,
                                            axiom_node_t *parent)
            {
              axis2_status_t status = AXIS2_SUCCESS;
            adb_statusType_set_statusType(_statusType,
                                                    env, node_value);
                  
              return status;
            }
        

        axis2_status_t AXIS2_CALL
        adb_statusType_deserialize(
                adb_statusType_t* _statusType,
                const axutil_env_t *env,
                axiom_node_t **dp_parent,
                axis2_bool_t *dp_is_early_node_valid,
                axis2_bool_t dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          axis2_status_t status = AXIS2_SUCCESS;
           
             axis2_char_t* text_value = NULL;
             axutil_qname_t *qname = NULL;
          
            axiom_element_t *text_element = NULL;
            axiom_node_t *text_node = NULL;
            
            status = AXIS2_FAILURE;
            if(parent)
            {
                text_node = axiom_node_get_first_child(parent, env);
                if (text_node &&
                        axiom_node_get_node_type(text_node, env) == AXIOM_TEXT)
                {
                    axiom_text_t *text_element = (axiom_text_t*)axiom_node_get_data_element(text_node, env);
                    if(text_element && axiom_text_get_value(text_element, env))
                    {
                        text_value = (axis2_char_t*)axiom_text_get_value(text_element, env);
                        status = adb_statusType_deserialize_from_string(_statusType, env, text_value, parent);
                    }
                }
            }
            
          return status;
       }

          axis2_bool_t AXIS2_CALL
          adb_statusType_is_particle()
          {
            
                 return AXIS2_FALSE;
              
          }


          void AXIS2_CALL
          adb_statusType_declare_parent_namespaces(
                    adb_statusType_t* _statusType,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
            axis2_char_t* AXIS2_CALL
            adb_statusType_serialize_to_string(
                    adb_statusType_t* _statusType,
                    const axutil_env_t *env, axutil_hash_t *namespaces)
            {
                axis2_char_t *text_value = NULL;
                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                
                       text_value = (axis2_char_t*)axutil_strdup(env, _statusType->property_statusType);
                    

                return text_value;
            }
        
        
        axiom_node_t* AXIS2_CALL
        adb_statusType_serialize(
                adb_statusType_t* _statusType,
                const axutil_env_t *env, axiom_node_t *parent, axiom_element_t *parent_element, int parent_tag_closed, axutil_hash_t *namespaces, int *next_ns_index)
        {
            
            
         
         axiom_node_t *current_node = NULL;
         int tag_closed = 0;

         
         
            axiom_data_source_t *data_source = NULL;
            axutil_stream_t *stream = NULL;
            axis2_char_t *text_value;
             
                    current_node = parent;
                    data_source = (axiom_data_source_t *)axiom_node_get_data_element(current_node, env);
                    if (!data_source)
                        return NULL;
                    stream = axiom_data_source_get_stream(data_source, env); /* assume parent is of type data source */
                    if (!stream)
                        return NULL;
                  
               if(!parent_tag_closed && !tag_closed)
               {
                  text_value = ">"; 
                  axutil_stream_write(stream, env, text_value, axutil_strlen(text_value));
               }
               
               text_value = adb_statusType_serialize_to_string(_statusType, env, namespaces);
               if(text_value)
               {
                    axutil_stream_write(stream, env, text_value, axutil_strlen(text_value));
                    AXIS2_FREE(env->allocator, text_value);
               }
            

            return parent;
        }


        

            /**
             * getter for statusType.
             */
            axis2_char_t* AXIS2_CALL
            adb_statusType_get_statusType(
                    adb_statusType_t* _statusType,
                    const axutil_env_t *env)
             {
                
                    AXIS2_ENV_CHECK(env, NULL);
                    AXIS2_PARAM_CHECK(env->error, _statusType, NULL);
                  

                return _statusType->property_statusType;
             }

            /**
             * setter for statusType
             */
            axis2_status_t AXIS2_CALL
            adb_statusType_set_statusType(
                    adb_statusType_t* _statusType,
                    const axutil_env_t *env,
                    const axis2_char_t*  arg_statusType)
             {
                

                AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
                AXIS2_PARAM_CHECK(env->error, _statusType, AXIS2_FAILURE);
                
                if(_statusType->is_valid_statusType &&
                        arg_statusType == _statusType->property_statusType)
                {
                    
                    return AXIS2_SUCCESS; 
                }

                
                  if(NULL == arg_statusType)
                  {
                      AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "statusType is being set to NULL, but it is not a nullable element");
                      return AXIS2_FAILURE;
                  }
                adb_statusType_reset_statusType(_statusType, env);

                
                if(NULL == arg_statusType)
                {
                    /* We are already done */
                    return AXIS2_SUCCESS;
                }
                _statusType->property_statusType = (axis2_char_t *)axutil_strdup(env, arg_statusType);
                        if(NULL == _statusType->property_statusType)
                        {
                            AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "Error allocating memeory for statusType");
                            return AXIS2_FAILURE;
                        }
                        _statusType->is_valid_statusType = AXIS2_TRUE;
                    
                return AXIS2_SUCCESS;
             }

             

           /**
            * resetter for statusType
            */
           axis2_status_t AXIS2_CALL
           adb_statusType_reset_statusType(
                   adb_statusType_t* _statusType,
                   const axutil_env_t *env)
           {
               int i = 0;
               int count = 0;
               void *element = NULL;

               AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
               AXIS2_PARAM_CHECK(env->error, _statusType, AXIS2_FAILURE);
               

               
            
                
                if(_statusType->property_statusType != NULL)
                {
                   
                   
                        AXIS2_FREE(env-> allocator, _statusType->property_statusType);
                     _statusType->property_statusType = NULL;
                }
            
                
                
                _statusType->is_valid_statusType = AXIS2_FALSE; 
               return AXIS2_SUCCESS;
           }

           /**
            * Check whether statusType is nill
            */
           axis2_bool_t AXIS2_CALL
           adb_statusType_is_statusType_nil(
                   adb_statusType_t* _statusType,
                   const axutil_env_t *env)
           {
               AXIS2_ENV_CHECK(env, AXIS2_TRUE);
               AXIS2_PARAM_CHECK(env->error, _statusType, AXIS2_TRUE);
               
               return !_statusType->is_valid_statusType;
           }

           /**
            * Set statusType to nill (currently the same as reset)
            */
           axis2_status_t AXIS2_CALL
           adb_statusType_set_statusType_nil(
                   adb_statusType_t* _statusType,
                   const axutil_env_t *env)
           {
               return adb_statusType_reset_statusType(_statusType, env);
           }

           

