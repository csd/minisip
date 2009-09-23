

        /**
        * axis2_stub_BoDServiceUserStubService.h
        *
        * This file was auto-generated from WSDL for "BoDServiceUserStubService|http://esb.bod.services.hdviper.i2cat.net/" service
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:35 LKT)
        */

        #include <stdio.h>
        #include <axiom.h>
        #include <axutil_utils.h>
        #include <axiom_soap.h>
        #include <axis2_client.h>
        #include <axis2_stub.h>

       
         #include "adb_modifyReservation1.h"
        
         #include "adb_modifyReservationResponse8.h"
        
         #include "adb_createReservation7.h"
        
         #include "adb_createReservationResponse3.h"
        
         #include "adb_getReservationStatuses4.h"
        
         #include "getReservationStatusesResponse5.h"
        
         #include "adb_cancelReservation6.h"
        
         #include "adb_cancelReservationResponse2.h"
        

	#ifdef __cplusplus
	extern "C" {
	#endif

        /***************** function prototypes - for header file *************/
        /**
         * axis2_stub_create_BoDServiceUserStubService
         * Create and return the stub with services populated
         * @param env Environment ( mandatory)
         * @param client_home Axis2/C home ( mandatory )
         * @param endpoint_uri Service endpoint uri( optional ) - if NULL default picked from WSDL used
         * @return Newly created stub object
         */
        axis2_stub_t*
        axis2_stub_create_BoDServiceUserStubService(const axutil_env_t *env,
                                        axis2_char_t *client_home,
                                        axis2_char_t *endpoint_uri);
        /**
         * axis2_stub_populate_services_for_BoDServiceUserStubService
         * populate the svc in stub with the service and operations
         * @param stub The stub
         * @param env environment ( mandatory)
         */
        void axis2_stub_populate_services_for_BoDServiceUserStubService( axis2_stub_t *stub, const axutil_env_t *env);
        /**
         * axis2_stub_get_endpoint_uri_of_BoDServiceUserStubService
         * Return the endpoint URI picked from WSDL
         * @param env environment ( mandatory)
         * @return The endpoint picked from WSDL
         */
        axis2_char_t *
        axis2_stub_get_endpoint_uri_of_BoDServiceUserStubService(const axutil_env_t *env);

        
            /**
             * Auto generated function declaration
             * for "modifyReservation|http://esb.bod.services.hdviper.i2cat.net/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _modifyReservation
             *
             * @return adb_modifyReservationResponse8_t*
             */


            adb_modifyReservationResponse8_t* 
            axis2_stub_op_BoDServiceUserStubService_modifyReservation( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_modifyReservation1_t* _modifyReservation);
          
            /**
             * Auto generated function declaration
             * for "createReservation|http://esb.bod.services.hdviper.i2cat.net/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _createReservation
             *
             * @return adb_createReservationResponse3_t*
             */


            adb_createReservationResponse3_t* 
            axis2_stub_op_BoDServiceUserStubService_createReservation( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_createReservation7_t* _createReservation);
          
            /**
             * Auto generated function declaration
             * for "getReservationStatuses|http://esb.bod.services.hdviper.i2cat.net/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _getReservationStatuses
             *
             * @return adb_getReservationStatusesResponse5_t*
             */


            adb_getReservationStatusesResponse5_t* 
            axis2_stub_op_BoDServiceUserStubService_getReservationStatuses( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_getReservationStatuses4_t* _getReservationStatuses);
          
            /**
             * Auto generated function declaration
             * for "cancelReservation|http://esb.bod.services.hdviper.i2cat.net/" operation.
             * @param stub The stub (axis2_stub_t)
             * @param env environment ( mandatory)
             *
             * @param _cancelReservation
             *
             * @return adb_cancelReservationResponse2_t*
             */


            adb_cancelReservationResponse2_t* 
            axis2_stub_op_BoDServiceUserStubService_cancelReservation( axis2_stub_t *stub, const axutil_env_t *env,
                                                        adb_cancelReservation6_t* _cancelReservation);
          
        /**
         * Auto generated function for asynchronous invocations
         * for "modifyReservation|http://esb.bod.services.hdviper.i2cat.net/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _modifyReservation
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_BoDServiceUserStubService_modifyReservation( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_modifyReservation1_t* _modifyReservation,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_modifyReservationResponse8_t* _modifyReservationResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

        
        /**
         * Auto generated function for asynchronous invocations
         * for "createReservation|http://esb.bod.services.hdviper.i2cat.net/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _createReservation
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_BoDServiceUserStubService_createReservation( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_createReservation7_t* _createReservation,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_createReservationResponse3_t* _createReservationResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

        
        /**
         * Auto generated function for asynchronous invocations
         * for "getReservationStatuses|http://esb.bod.services.hdviper.i2cat.net/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _getReservationStatuses
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_BoDServiceUserStubService_getReservationStatuses( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_getReservationStatuses4_t* _getReservationStatuses,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_getReservationStatusesResponse5_t* _getReservationStatusesResponse, void *data) ,
                                                  axis2_status_t ( AXIS2_CALL *on_error ) (const axutil_env_t *, int exception, void *data) );

        
        /**
         * Auto generated function for asynchronous invocations
         * for "cancelReservation|http://esb.bod.services.hdviper.i2cat.net/" operation.
         * @param stub The stub
         * @param env environment ( mandatory)
         
         * @param _cancelReservation
         * @param user_data user data to be accessed by the callbacks
         * @param on_complete callback to handle on complete
         * @param on_error callback to handle on error
         */

        

        void axis2_stub_start_op_BoDServiceUserStubService_cancelReservation( axis2_stub_t *stub, const axutil_env_t *env,
                                                    adb_cancelReservation6_t* _cancelReservation,
                                                  void *user_data,
                                                  axis2_status_t ( AXIS2_CALL *on_complete ) (const axutil_env_t *, adb_cancelReservationResponse2_t* _cancelReservationResponse, void *data) ,
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
   

