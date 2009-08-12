

        /**
        * axis2_stub_PresenceAgentUserStubService.h
        *
        * This file was auto-generated from WSDL for "PresenceAgentUserStubService|http://esb.presenceagent.services.hdviper.psnc.pl/" service
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:35 LKT)
        */

        #include <stdio.h>
        #include <axiom.h>
        #include <axutil_utils.h>
        #include <axiom_soap.h>
        #include <axis2_client.h>
        #include <axis2_stub.h>

       
         #include "adb_getStatus2.h"
        
         #include "adb_getStatusResponse3.h"
        
         #include "adb_setStatus7.h"
        
         #include "adb_setStatusResponse6.h"
        
         #include "adb_setContactList8.h"
        
         #include "adb_setContactListResponse1.h"
        
         #include "adb_getStatuses10.h"
        
         #include "adb_getStatusesResponse0.h"
        

	#ifdef __cplusplus
	extern "C" {
	#endif

        /***************** function prototypes - for header file *************/
        /**
         * axis2_stub_create_PresenceAgentUserStubService
         * Create and return the stub with services populated
         * @param env Environment ( mandatory)
         * @param client_home Axis2/C home ( mandatory )
         * @param endpoint_uri Service endpoint uri( optional ) - if NULL default picked from WSDL used
         * @return Newly created stub object
         */
        axis2_stub_t*
        axis2_stub_create_PresenceAgentUserStubService(const axutil_env_t *env,
                                        axis2_char_t *client_home,
                                        axis2_char_t *endpoint_uri);
        /**
         * axis2_stub_populate_services_for_PresenceAgentUserStubService
         * populate the svc in stub with the service and operations
         * @param stub The stub
         * @param env environment ( mandatory)
         */
        void axis2_stub_populate_services_for_PresenceAgentUserStubService( axis2_stub_t *stub, const axutil_env_t *env);
        /**
         * axis2_stub_get_endpoint_uri_of_PresenceAgentUserStubService
         * Return the endpoint URI picked from WSDL
         * @param env environment ( mandatory)
         * @return The endpoint picked from WSDL
         */
        axis2_char_t *
        axis2_stub_get_endpoint_uri_of_PresenceAgentUserStubService(const axutil_env_t *env);

        
            /**
             * Auto generated function declaration
             * for "getStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _getStatus
             *
             * @return adb_getStatusResponse3_t*
             */


            adb_getStatusResponse3_t* 
            axis2_stub_op_PresenceAgentUserStubService_getStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_getStatus2_t* _getStatus);
          
            /**
             * Auto generated function declaration
             * for "setStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _setStatus
             *
             * @return adb_setStatusResponse6_t*
             */


            adb_setStatusResponse6_t* 
            axis2_stub_op_PresenceAgentUserStubService_setStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_setStatus7_t* _setStatus);
          
            /**
             * Auto generated function declaration
             * for "setContactList|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _setContactList
             *
             * @return adb_setContactListResponse1_t*
             */


            adb_setContactListResponse1_t* 
            axis2_stub_op_PresenceAgentUserStubService_setContactList( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_setContactList8_t* _setContactList);
          
            /**
             * Auto generated function declaration
             * for "getStatuses|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _getStatuses
             *
             * @return adb_getStatusesResponse0_t*
             */


            adb_getStatusesResponse0_t* 
            axis2_stub_op_PresenceAgentUserStubService_getStatuses( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_getStatuses10_t* _getStatuses);
          
        /**
         * Auto generated function for asynchronous invocations
         * for "getStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _getStatus
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_PresenceAgentUserStubService_getStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_getStatus2_t* _getStatus,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusResponse3_t* _getStatusResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

        
        /**
         * Auto generated function for asynchronous invocations
         * for "setStatus|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _setStatus
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_PresenceAgentUserStubService_setStatus( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_setStatus7_t* _setStatus,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setStatusResponse6_t* _setStatusResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

        
        /**
         * Auto generated function for asynchronous invocations
         * for "setContactList|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _setContactList
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_PresenceAgentUserStubService_setContactList( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_setContactList8_t* _setContactList,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_setContactListResponse1_t* _setContactListResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

        
        /**
         * Auto generated function for asynchronous invocations
         * for "getStatuses|http://esb.presenceagent.services.hdviper.psnc.pl/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _getStatuses
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_PresenceAgentUserStubService_getStatuses( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_getStatuses10_t* _getStatuses,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getStatusesResponse0_t* _getStatusesResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

          

     /**
      * function to free any soap input headers 
      * @param env environment ( mandatory)
      */
     

     /**
      * function to free any soap output headers 
      * @param env environment ( mandatory)
      */
     

	#ifdef __cplusplus
	}
	#endif
   

