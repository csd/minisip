

      /**
       * axis2_stub_PresenceAgentUserStubService.c
       *
       * This file was auto-generated from WSDL for "PresenceAgentUserStubService|http://esb.presenceagent.services.hdviper.psnc.pl/" service
       * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:35 LKT)
       */

      #include "stub_PresenceAgentUserStubService.h"
      #include <axis2_msg.h>

      /**
       * axis2_stub_PresenceAgentUserStubService C implementation
       */

      axis2_stub_t*
      axis2_stub_create_PresenceAgentUserStubService(const axutil_env_t *env,
                                      axis2_char_t *client_home,
                                      axis2_char_t *endpoint_uri)
      {
         axis2_stub_t *stub = NULL;
         axis2_endpoint_ref_t *endpoint_ref = NULL;
         AXIS2_FUNC_PARAM_CHECK (client_home, env, NULL)

         if (NULL == endpoint_uri)
         {
            endpoint_uri = axis2_stub_get_endpoint_uri_of_PresenceAgentUserStubService(env);
         }

         endpoint_ref = axis2_endpoint_ref_create(env, endpoint_uri);

         stub = axis2_stub_create_with_endpoint_ref_and_client_home (env, endpoint_ref, client_home);

         if (NULL == stub)
         {
            if(NULL != endpoint_ref)
            {
                axis2_endpoint_ref_free(endpoint_ref, env);
            }
            return NULL;
         }


         axis2_stub_populate_services_for_PresenceAgentUserStubService(stub, env);
         return stub;
      }


      void
      axis2_stub_populate_services_for_PresenceAgentUserStubService(axis2_stub_t *stub, const axutil_env_t *env)
      {
         axis2_svc_client_t *svc_client = NULL;
         axutil_qname_t *svc_qname =  NULL;
         axutil_qname_t *op_qname =  NULL;
         axis2_svc_t *svc = NULL;
         axis2_op_t *op = NULL;
         axis2_op_t *annon_op = NULL;
         axis2_msg_t *msg_out = NULL;
         axis2_msg_t *msg_in = NULL;
         axis2_msg_t *msg_out_fault = NULL;
         axis2_msg_t *msg_in_fault = NULL;


         /* Modifying the Service */
         svc_client = axis2_stub_get_svc_client (stub, env );
         svc = (axis2_svc_t*)axis2_svc_client_get_svc( svc_client, env );

         annon_op = axis2_svc_get_op_with_name(svc, env, AXIS2_ANON_OUT_IN_OP);
         msg_out = axis2_op_get_msg(annon_op, env, AXIS2_MSG_OUT);
         msg_in = axis2_op_get_msg(annon_op, env, AXIS2_MSG_IN);
         msg_out_fault = axis2_op_get_msg(annon_op, env, AXIS2_MSG_OUT_FAULT);
         msg_in_fault = axis2_op_get_msg(annon_op, env, AXIS2_MSG_IN_FAULT);

         svc_qname = axutil_qname_create(env,"PresenceAgentUserStubService" ,NULL, NULL);
         axis2_svc_set_qname (svc, env, svc_qname);

         /* creating the operations*/

         
           op_qname = axutil_qname_create(env,
                                         "getStatus" ,
                                         "http://esb.presenceagent.services.hdviper.psnc.pl/",
                                         NULL);
           op = axis2_op_create_with_qname(env, op_qname);
           
               axis2_op_set_msg_exchange_pattern(op, env, AXIS2_MEP_URI_OUT_IN);
             
           axis2_msg_increment_ref(msg_out, env);
           axis2_msg_increment_ref(msg_in, env);
           axis2_msg_increment_ref(msg_out_fault, env);
           axis2_msg_increment_ref(msg_in_fault, env);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT, msg_out);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN, msg_in);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT_FAULT, msg_out_fault);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN_FAULT, msg_in_fault);
           
           axis2_svc_add_op(svc, env, op);

         
           op_qname = axutil_qname_create(env,
                                         "setStatus" ,
                                         "http://esb.presenceagent.services.hdviper.psnc.pl/",
                                         NULL);
           op = axis2_op_create_with_qname(env, op_qname);
           
               axis2_op_set_msg_exchange_pattern(op, env, AXIS2_MEP_URI_OUT_IN);
             
           axis2_msg_increment_ref(msg_out, env);
           axis2_msg_increment_ref(msg_in, env);
           axis2_msg_increment_ref(msg_out_fault, env);
           axis2_msg_increment_ref(msg_in_fault, env);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT, msg_out);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN, msg_in);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT_FAULT, msg_out_fault);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN_FAULT, msg_in_fault);
           
           axis2_svc_add_op(svc, env, op);

         
           op_qname = axutil_qname_create(env,
                                         "setContactList" ,
                                         "http://esb.presenceagent.services.hdviper.psnc.pl/",
                                         NULL);
           op = axis2_op_create_with_qname(env, op_qname);
           
               axis2_op_set_msg_exchange_pattern(op, env, AXIS2_MEP_URI_OUT_IN);
             
           axis2_msg_increment_ref(msg_out, env);
           axis2_msg_increment_ref(msg_in, env);
           axis2_msg_increment_ref(msg_out_fault, env);
           axis2_msg_increment_ref(msg_in_fault, env);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT, msg_out);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN, msg_in);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT_FAULT, msg_out_fault);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN_FAULT, msg_in_fault);
           
           axis2_svc_add_op(svc, env, op);

         
           op_qname = axutil_qname_create(env,
                                         "getStatuses" ,
                                         "http://esb.presenceagent.services.hdviper.psnc.pl/",
                                         NULL);
           op = axis2_op_create_with_qname(env, op_qname);
           
               axis2_op_set_msg_exchange_pattern(op, env, AXIS2_MEP_URI_OUT_IN);
             
           axis2_msg_increment_ref(msg_out, env);
           axis2_msg_increment_ref(msg_in, env);
           axis2_msg_increment_ref(msg_out_fault, env);
           axis2_msg_increment_ref(msg_in_fault, env);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT, msg_out);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN, msg_in);
           axis2_op_add_msg(op, env, AXIS2_MSG_OUT_FAULT, msg_out_fault);
           axis2_op_add_msg(op, env, AXIS2_MSG_IN_FAULT, msg_in_fault);
           
           axis2_svc_add_op(svc, env, op);

         
      }

      /**
       *return end point picked from wsdl
       */
      axis2_char_t*
      axis2_stub_get_endpoint_uri_of_PresenceAgentUserStubService( const axutil_env_t *env )
      {
        axis2_char_t *endpoint_uri = NULL;
        /* set the address from here */
        
              endpoint_uri = "http://esb.hdviper.org:8080/hdviper-presenceagent-us/PresenceAgentUserStub";
            
        return endpoint_uri;
      }


  
         /**
          * auto generated method signature
          * for "getStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          *
          * @param _getStatus
          *
          * @return adb_getStatusResponse3_t*
          */
         adb_getStatusResponse3_t* 
         axis2_stub_op_PresenceAgentUserStubService_getStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                              adb_getStatus2_t* _getStatus)
         {
            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;
            axiom_node_t *ret_node = NULL;

            const axis2_char_t *soap_action = NULL;
            axutil_qname_t *op_qname =  NULL;
            axiom_node_t *payload = NULL;
            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            adb_getStatusResponse3_t* ret_val = NULL;
            
                       payload = adb_getStatus2_serialize(_getStatus, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   
            svc_client = axis2_stub_get_svc_client(stub, env );
            
           
            
            

            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "options is null in stub");
                return NULL;
            }
            soap_action = axis2_options_get_action( options, env );
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);    
              
              axis2_options_set_action( options, env, soap_action );
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11 );
            
            ret_node =  axis2_svc_client_send_receive_with_op_qname( svc_client, env, op_qname, payload);
 
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);    
              
              axis2_options_set_action( options, env, NULL);
            }

            
            
                    if ( NULL == ret_node )
                    {
                        return NULL;
                    }
                    ret_val = adb_getStatusResponse3_create(env);

                    if(adb_getStatusResponse3_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                    {
                        if(ret_val != NULL)
                        {
                            adb_getStatusResponse3_free(ret_val, env);
                        }

                        AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the getStatusResponse3_deserialize: "
                                                                "This should be due to an invalid XML");
                        return NULL;
                    }
                    return ret_val;
                
        }
        
         /**
          * auto generated method signature
          * for "setStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          *
          * @param _setStatus
          *
          * @return adb_setStatusResponse6_t*
          */
         adb_setStatusResponse6_t* 
         axis2_stub_op_PresenceAgentUserStubService_setStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                              adb_setStatus7_t* _setStatus)
         {
            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;
            axiom_node_t *ret_node = NULL;

            const axis2_char_t *soap_action = NULL;
            axutil_qname_t *op_qname =  NULL;
            axiom_node_t *payload = NULL;
            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            adb_setStatusResponse6_t* ret_val = NULL;
            
                       payload = adb_setStatus7_serialize(_setStatus, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   
            svc_client = axis2_stub_get_svc_client(stub, env );
            
           
            
            

            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "options is null in stub");
                return NULL;
            }
            soap_action = axis2_options_get_action( options, env );
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);    
              
              axis2_options_set_action( options, env, soap_action );
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11 );
            
            ret_node =  axis2_svc_client_send_receive_with_op_qname( svc_client, env, op_qname, payload);
 
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);    
              
              axis2_options_set_action( options, env, NULL);
            }

            
            
                    if ( NULL == ret_node )
                    {
                        return NULL;
                    }
                    ret_val = adb_setStatusResponse6_create(env);

                    if(adb_setStatusResponse6_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                    {
                        if(ret_val != NULL)
                        {
                            adb_setStatusResponse6_free(ret_val, env);
                        }

                        AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the setStatusResponse6_deserialize: "
                                                                "This should be due to an invalid XML");
                        return NULL;
                    }
                    return ret_val;
                
        }
        
         /**
          * auto generated method signature
          * for "setContactList|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          *
          * @param _setContactList
          *
          * @return adb_setContactListResponse1_t*
          */
         adb_setContactListResponse1_t* 
         axis2_stub_op_PresenceAgentUserStubService_setContactList( axis2_stub_t *stub, const axutil_env_t *env,
                                              adb_setContactList8_t* _setContactList)
         {
            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;
            axiom_node_t *ret_node = NULL;

            const axis2_char_t *soap_action = NULL;
            axutil_qname_t *op_qname =  NULL;
            axiom_node_t *payload = NULL;
            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            adb_setContactListResponse1_t* ret_val = NULL;
            
                       payload = adb_setContactList8_serialize(_setContactList, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   
            svc_client = axis2_stub_get_svc_client(stub, env );
            
           
            
            

            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "options is null in stub");
                return NULL;
            }
            soap_action = axis2_options_get_action( options, env );
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);    
              
              axis2_options_set_action( options, env, soap_action );
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11 );
            
            ret_node =  axis2_svc_client_send_receive_with_op_qname( svc_client, env, op_qname, payload);
 
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);    
              
              axis2_options_set_action( options, env, NULL);
            }

            
            
                    if ( NULL == ret_node )
                    {
                        return NULL;
                    }
                    ret_val = adb_setContactListResponse1_create(env);

                    if(adb_setContactListResponse1_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                    {
                        if(ret_val != NULL)
                        {
                            adb_setContactListResponse1_free(ret_val, env);
                        }

                        AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the setContactListResponse1_deserialize: "
                                                                "This should be due to an invalid XML");
                        return NULL;
                    }
                    return ret_val;
                
        }
        
         /**
          * auto generated method signature
          * for "getStatuses|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          *
          * @param _getStatuses
          *
          * @return adb_getStatusesResponse0_t*
          */
         adb_getStatusesResponse0_t* 
         axis2_stub_op_PresenceAgentUserStubService_getStatuses( axis2_stub_t *stub, const axutil_env_t *env,
                                              adb_getStatuses10_t* _getStatuses)
         {
            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;
            axiom_node_t *ret_node = NULL;

            const axis2_char_t *soap_action = NULL;
            axutil_qname_t *op_qname =  NULL;
            axiom_node_t *payload = NULL;
            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            adb_getStatusesResponse0_t* ret_val = NULL;
            
                       payload = adb_getStatuses10_serialize(_getStatuses, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   
            svc_client = axis2_stub_get_svc_client(stub, env );
            
           
            
            

            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
                AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
                AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI, "options is null in stub");
                return NULL;
            }
            soap_action = axis2_options_get_action( options, env );
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);    
              
              axis2_options_set_action( options, env, soap_action );
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11 );
            
            ret_node =  axis2_svc_client_send_receive_with_op_qname( svc_client, env, op_qname, payload);
 
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);    
              
              axis2_options_set_action( options, env, NULL);
            }

            
            
                    if ( NULL == ret_node )
                    {
                        return NULL;
                    }
                    ret_val = adb_getStatusesResponse0_create(env);

                    if(adb_getStatusesResponse0_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                    {
                        if(ret_val != NULL)
                        {
                            adb_getStatusesResponse0_free(ret_val, env);
                        }

                        AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the getStatusesResponse0_deserialize: "
                                                                "This should be due to an invalid XML");
                        return NULL;
                    }
                    return ret_val;
                
        }
        

        struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data
        {   
            void *data;
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusResponse3_t* _getStatusResponse, void *data);
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data);
        };

        static axis2_status_t AXIS2_CALL axis2_stub_on_error_PresenceAgentUserStubService_getStatus(axis2_callback_t *callback, const axutil_env_t *env, int exception)
        {
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int, void *data);
            struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data* callback_data = NULL;
            void *user_data = NULL;

            axis2_status_t status;
        
            callback_data = (struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data*)axis2_callback_get_data(callback);
        
            user_data = callback_data->data;
            on_error = callback_data->on_error;
        
            status = on_error(env, exception, user_data);

            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        } 

        axis2_status_t AXIS2_CALL axis2_stub_on_complete_PresenceAgentUserStubService_getStatus(axis2_callback_t *callback, const axutil_env_t *env)
        {
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusResponse3_t* _getStatusResponse, void *data);
            struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data* callback_data = NULL;
            void *user_data = NULL;
            axis2_status_t status = AXIS2_SUCCESS;
 
            adb_getStatusResponse3_t* ret_val = NULL;
            

            axiom_node_t *ret_node = NULL;
            axiom_soap_envelope_t *soap_envelope = NULL;

            

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data*)axis2_callback_get_data(callback);

            soap_envelope = axis2_callback_get_envelope(callback, env);
            if(soap_envelope)
            {
                axiom_soap_body_t *soap_body;
                soap_body = axiom_soap_envelope_get_body(soap_envelope, env);
                if(soap_body)
                {
                    axiom_node_t *body_node = axiom_soap_body_get_base_node(soap_body, env);
                    if(body_node)
                    {
                        ret_node = axiom_node_get_first_child(body_node, env);
                    }
                }
                
                
            }

            user_data = callback_data->data;
            on_complete = callback_data->on_complete;

            
                    if(ret_node != NULL)
                    {
                        ret_val = adb_getStatusResponse3_create(env);
     
                        if(adb_getStatusResponse3_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                        {
                            AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the LendResponse_deserialize: "
                                                                    "This should be due to an invalid XML");
                            adb_getStatusResponse3_free(ret_val, env);
                            ret_val = NULL;
                        }
                     }
                     else
                     {
                         ret_val = NULL; 
                     }
                     status = on_complete(env, ret_val, user_data);
                
 
            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        }

        /**
          * auto generated method signature for asynchronous invocations
          * for "getStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          
          *
          * @param _getStatus
          * @param on_complete callback to handle on complete
          * @param on_error callback to handle on error
          */
         void axis2_stub_start_op_PresenceAgentUserStubService_getStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_getStatus2_t* _getStatus,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusResponse3_t* _getStatusResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) )
         {

            axis2_callback_t *callback = NULL;

            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;

            const axis2_char_t *soap_action = NULL;
            axiom_node_t *payload = NULL;

            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            
            
            struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data *callback_data;

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data*) AXIS2_MALLOC(env->allocator, 
                                    sizeof(struct axis2_stub_PresenceAgentUserStubService_getStatus_callback_data));
            if(NULL == callback_data)
            {
                AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "Can not allocate memeory for the callback data structures");
                return;
            }
            
            
                       payload = adb_getStatus2_serialize(_getStatus, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   



            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
              AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
              AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "options is null in stub");
              return;
            }
            svc_client = axis2_stub_get_svc_client (stub, env);
            soap_action =axis2_options_get_action (options, env);
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);
              
              axis2_options_set_action( options, env, soap_action);
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11);
             

            callback = axis2_callback_create(env);
            /* Set our on_complete fucntion pointer to the callback object */
            axis2_callback_set_on_complete(callback, axis2_stub_on_complete_PresenceAgentUserStubService_getStatus);
            /* Set our on_error function pointer to the callback object */
            axis2_callback_set_on_error(callback, axis2_stub_on_error_PresenceAgentUserStubService_getStatus);

            callback_data-> data = user_data;
            callback_data-> on_complete = on_complete;
            callback_data-> on_error = on_error;

            axis2_callback_set_data(callback, (void*)callback_data);

            /* Send request */
            axis2_svc_client_send_receive_non_blocking(svc_client, env, payload, callback);
            
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);
              
              axis2_options_set_action(options, env, NULL);
            }
         }

         

        struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data
        {   
            void *data;
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setStatusResponse6_t* _setStatusResponse, void *data);
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data);
        };

        static axis2_status_t AXIS2_CALL axis2_stub_on_error_PresenceAgentUserStubService_setStatus(axis2_callback_t *callback, const axutil_env_t *env, int exception)
        {
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int, void *data);
            struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data* callback_data = NULL;
            void *user_data = NULL;

            axis2_status_t status;
        
            callback_data = (struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data*)axis2_callback_get_data(callback);
        
            user_data = callback_data->data;
            on_error = callback_data->on_error;
        
            status = on_error(env, exception, user_data);

            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        } 

        axis2_status_t AXIS2_CALL axis2_stub_on_complete_PresenceAgentUserStubService_setStatus(axis2_callback_t *callback, const axutil_env_t *env)
        {
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setStatusResponse6_t* _setStatusResponse, void *data);
            struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data* callback_data = NULL;
            void *user_data = NULL;
            axis2_status_t status = AXIS2_SUCCESS;
 
            adb_setStatusResponse6_t* ret_val = NULL;
            

            axiom_node_t *ret_node = NULL;
            axiom_soap_envelope_t *soap_envelope = NULL;

            

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data*)axis2_callback_get_data(callback);

            soap_envelope = axis2_callback_get_envelope(callback, env);
            if(soap_envelope)
            {
                axiom_soap_body_t *soap_body;
                soap_body = axiom_soap_envelope_get_body(soap_envelope, env);
                if(soap_body)
                {
                    axiom_node_t *body_node = axiom_soap_body_get_base_node(soap_body, env);
                    if(body_node)
                    {
                        ret_node = axiom_node_get_first_child(body_node, env);
                    }
                }
                
                
            }

            user_data = callback_data->data;
            on_complete = callback_data->on_complete;

            
                    if(ret_node != NULL)
                    {
                        ret_val = adb_setStatusResponse6_create(env);
     
                        if(adb_setStatusResponse6_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                        {
                            AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the LendResponse_deserialize: "
                                                                    "This should be due to an invalid XML");
                            adb_setStatusResponse6_free(ret_val, env);
                            ret_val = NULL;
                        }
                     }
                     else
                     {
                         ret_val = NULL; 
                     }
                     status = on_complete(env, ret_val, user_data);
                
 
            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        }

        /**
          * auto generated method signature for asynchronous invocations
          * for "setStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          
          *
          * @param _setStatus
          * @param on_complete callback to handle on complete
          * @param on_error callback to handle on error
          */
         void axis2_stub_start_op_PresenceAgentUserStubService_setStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_setStatus7_t* _setStatus,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setStatusResponse6_t* _setStatusResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) )
         {

            axis2_callback_t *callback = NULL;

            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;

            const axis2_char_t *soap_action = NULL;
            axiom_node_t *payload = NULL;

            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            
            
            struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data *callback_data;

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data*) AXIS2_MALLOC(env->allocator, 
                                    sizeof(struct axis2_stub_PresenceAgentUserStubService_setStatus_callback_data));
            if(NULL == callback_data)
            {
                AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "Can not allocate memeory for the callback data structures");
                return;
            }
            
            
                       payload = adb_setStatus7_serialize(_setStatus, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   



            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
              AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
              AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "options is null in stub");
              return;
            }
            svc_client = axis2_stub_get_svc_client (stub, env);
            soap_action =axis2_options_get_action (options, env);
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);
              
              axis2_options_set_action( options, env, soap_action);
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11);
             

            callback = axis2_callback_create(env);
            /* Set our on_complete fucntion pointer to the callback object */
            axis2_callback_set_on_complete(callback, axis2_stub_on_complete_PresenceAgentUserStubService_setStatus);
            /* Set our on_error function pointer to the callback object */
            axis2_callback_set_on_error(callback, axis2_stub_on_error_PresenceAgentUserStubService_setStatus);

            callback_data-> data = user_data;
            callback_data-> on_complete = on_complete;
            callback_data-> on_error = on_error;

            axis2_callback_set_data(callback, (void*)callback_data);

            /* Send request */
            axis2_svc_client_send_receive_non_blocking(svc_client, env, payload, callback);
            
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);
              
              axis2_options_set_action(options, env, NULL);
            }
         }

         

        struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data
        {   
            void *data;
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setContactListResponse1_t* _setContactListResponse, void *data);
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data);
        };

        static axis2_status_t AXIS2_CALL axis2_stub_on_error_PresenceAgentUserStubService_setContactList(axis2_callback_t *callback, const axutil_env_t *env, int exception)
        {
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int, void *data);
            struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data* callback_data = NULL;
            void *user_data = NULL;

            axis2_status_t status;
        
            callback_data = (struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data*)axis2_callback_get_data(callback);
        
            user_data = callback_data->data;
            on_error = callback_data->on_error;
        
            status = on_error(env, exception, user_data);

            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        } 

        axis2_status_t AXIS2_CALL axis2_stub_on_complete_PresenceAgentUserStubService_setContactList(axis2_callback_t *callback, const axutil_env_t *env)
        {
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setContactListResponse1_t* _setContactListResponse, void *data);
            struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data* callback_data = NULL;
            void *user_data = NULL;
            axis2_status_t status = AXIS2_SUCCESS;
 
            adb_setContactListResponse1_t* ret_val = NULL;
            

            axiom_node_t *ret_node = NULL;
            axiom_soap_envelope_t *soap_envelope = NULL;

            

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data*)axis2_callback_get_data(callback);

            soap_envelope = axis2_callback_get_envelope(callback, env);
            if(soap_envelope)
            {
                axiom_soap_body_t *soap_body;
                soap_body = axiom_soap_envelope_get_body(soap_envelope, env);
                if(soap_body)
                {
                    axiom_node_t *body_node = axiom_soap_body_get_base_node(soap_body, env);
                    if(body_node)
                    {
                        ret_node = axiom_node_get_first_child(body_node, env);
                    }
                }
                
                
            }

            user_data = callback_data->data;
            on_complete = callback_data->on_complete;

            
                    if(ret_node != NULL)
                    {
                        ret_val = adb_setContactListResponse1_create(env);
     
                        if(adb_setContactListResponse1_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                        {
                            AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the LendResponse_deserialize: "
                                                                    "This should be due to an invalid XML");
                            adb_setContactListResponse1_free(ret_val, env);
                            ret_val = NULL;
                        }
                     }
                     else
                     {
                         ret_val = NULL; 
                     }
                     status = on_complete(env, ret_val, user_data);
                
 
            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        }

        /**
          * auto generated method signature for asynchronous invocations
          * for "setContactList|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          
          *
          * @param _setContactList
          * @param on_complete callback to handle on complete
          * @param on_error callback to handle on error
          */
         void axis2_stub_start_op_PresenceAgentUserStubService_setContactList( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_setContactList8_t* _setContactList,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setContactListResponse1_t* _setContactListResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) )
         {

            axis2_callback_t *callback = NULL;

            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;

            const axis2_char_t *soap_action = NULL;
            axiom_node_t *payload = NULL;

            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            
            
            struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data *callback_data;

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data*) AXIS2_MALLOC(env->allocator, 
                                    sizeof(struct axis2_stub_PresenceAgentUserStubService_setContactList_callback_data));
            if(NULL == callback_data)
            {
                AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "Can not allocate memeory for the callback data structures");
                return;
            }
            
            
                       payload = adb_setContactList8_serialize(_setContactList, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   



            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
              AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
              AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "options is null in stub");
              return;
            }
            svc_client = axis2_stub_get_svc_client (stub, env);
            soap_action =axis2_options_get_action (options, env);
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);
              
              axis2_options_set_action( options, env, soap_action);
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11);
             

            callback = axis2_callback_create(env);
            /* Set our on_complete fucntion pointer to the callback object */
            axis2_callback_set_on_complete(callback, axis2_stub_on_complete_PresenceAgentUserStubService_setContactList);
            /* Set our on_error function pointer to the callback object */
            axis2_callback_set_on_error(callback, axis2_stub_on_error_PresenceAgentUserStubService_setContactList);

            callback_data-> data = user_data;
            callback_data-> on_complete = on_complete;
            callback_data-> on_error = on_error;

            axis2_callback_set_data(callback, (void*)callback_data);

            /* Send request */
            axis2_svc_client_send_receive_non_blocking(svc_client, env, payload, callback);
            
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);
              
              axis2_options_set_action(options, env, NULL);
            }
         }

         

        struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data
        {   
            void *data;
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusesResponse0_t* _getStatusesResponse, void *data);
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data);
        };

        static axis2_status_t AXIS2_CALL axis2_stub_on_error_PresenceAgentUserStubService_getStatuses(axis2_callback_t *callback, const axutil_env_t *env, int exception)
        {
            axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int, void *data);
            struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data* callback_data = NULL;
            void *user_data = NULL;

            axis2_status_t status;
        
            callback_data = (struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data*)axis2_callback_get_data(callback);
        
            user_data = callback_data->data;
            on_error = callback_data->on_error;
        
            status = on_error(env, exception, user_data);

            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        } 

        axis2_status_t AXIS2_CALL axis2_stub_on_complete_PresenceAgentUserStubService_getStatuses(axis2_callback_t *callback, const axutil_env_t *env)
        {
            axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusesResponse0_t* _getStatusesResponse, void *data);
            struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data* callback_data = NULL;
            void *user_data = NULL;
            axis2_status_t status = AXIS2_SUCCESS;
 
            adb_getStatusesResponse0_t* ret_val = NULL;
            

            axiom_node_t *ret_node = NULL;
            axiom_soap_envelope_t *soap_envelope = NULL;

            

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data*)axis2_callback_get_data(callback);

            soap_envelope = axis2_callback_get_envelope(callback, env);
            if(soap_envelope)
            {
                axiom_soap_body_t *soap_body;
                soap_body = axiom_soap_envelope_get_body(soap_envelope, env);
                if(soap_body)
                {
                    axiom_node_t *body_node = axiom_soap_body_get_base_node(soap_body, env);
                    if(body_node)
                    {
                        ret_node = axiom_node_get_first_child(body_node, env);
                    }
                }
                
                
            }

            user_data = callback_data->data;
            on_complete = callback_data->on_complete;

            
                    if(ret_node != NULL)
                    {
                        ret_val = adb_getStatusesResponse0_create(env);
     
                        if(adb_getStatusesResponse0_deserialize(ret_val, env, &ret_node, NULL, AXIS2_FALSE ) == AXIS2_FAILURE)
                        {
                            AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "NULL returnted from the LendResponse_deserialize: "
                                                                    "This should be due to an invalid XML");
                            adb_getStatusesResponse0_free(ret_val, env);
                            ret_val = NULL;
                        }
                     }
                     else
                     {
                         ret_val = NULL; 
                     }
                     status = on_complete(env, ret_val, user_data);
                
 
            if(callback_data)
            {
                AXIS2_FREE(env->allocator, callback_data);
            }
            return status;
        }

        /**
          * auto generated method signature for asynchronous invocations
          * for "getStatuses|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
          
          *
          * @param _getStatuses
          * @param on_complete callback to handle on complete
          * @param on_error callback to handle on error
          */
         void axis2_stub_start_op_PresenceAgentUserStubService_getStatuses( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_getStatuses10_t* _getStatuses,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusesResponse0_t* _getStatusesResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) )
         {

            axis2_callback_t *callback = NULL;

            axis2_svc_client_t *svc_client = NULL;
            axis2_options_t *options = NULL;

            const axis2_char_t *soap_action = NULL;
            axiom_node_t *payload = NULL;

            axis2_bool_t is_soap_act_set = AXIS2_TRUE;
            
            axutil_string_t *soap_act = NULL;
            
            
            struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data *callback_data;

            callback_data = (struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data*) AXIS2_MALLOC(env->allocator, 
                                    sizeof(struct axis2_stub_PresenceAgentUserStubService_getStatuses_callback_data));
            if(NULL == callback_data)
            {
                AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "Can not allocate memeory for the callback data structures");
                return;
            }
            
            
                       payload = adb_getStatuses10_serialize(_getStatuses, env, NULL, NULL, AXIS2_TRUE, NULL, NULL);
                   



            options = axis2_stub_get_options( stub, env);
            if (NULL == options)
            {
              AXIS2_ERROR_SET(env->error, AXIS2_ERROR_INVALID_NULL_PARAM, AXIS2_FAILURE);
              AXIS2_LOG_ERROR( env->log, AXIS2_LOG_SI, "options is null in stub");
              return;
            }
            svc_client = axis2_stub_get_svc_client (stub, env);
            soap_action =axis2_options_get_action (options, env);
            if (NULL == soap_action)
            {
              is_soap_act_set = AXIS2_FALSE;
              soap_action = "\"\"";
              
              soap_act = axutil_string_create(env, "\"\"");
              axis2_options_set_soap_action(options, env, soap_act);
              
              axis2_options_set_action( options, env, soap_action);
            }
            
            axis2_options_set_soap_version(options, env, AXIOM_SOAP11);
             

            callback = axis2_callback_create(env);
            /* Set our on_complete fucntion pointer to the callback object */
            axis2_callback_set_on_complete(callback, axis2_stub_on_complete_PresenceAgentUserStubService_getStatuses);
            /* Set our on_error function pointer to the callback object */
            axis2_callback_set_on_error(callback, axis2_stub_on_error_PresenceAgentUserStubService_getStatuses);

            callback_data-> data = user_data;
            callback_data-> on_complete = on_complete;
            callback_data-> on_error = on_error;

            axis2_callback_set_data(callback, (void*)callback_data);

            /* Send request */
            axis2_svc_client_send_receive_non_blocking(svc_client, env, payload, callback);
            
            if (!is_soap_act_set)
            {
              
              axis2_options_set_soap_action(options, env, NULL);
              
              axis2_options_set_action(options, env, NULL);
            }
         }

            


     /**
      * function to free any soap input headers 
      */
     


     /**
      * function to free any soap output headers 
      */
     

