

        #ifndef ADB_CREATERESERVATIONRESPONSE3_H
        #define ADB_CREATERESERVATIONRESPONSE3_H

       /**
        * adb_createReservationResponse3.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_createReservationResponse3 class
        */
        typedef struct adb_createReservationResponse3 adb_createReservationResponse3_t;

        
          #include "adb_createReservationResponse.h"
          

        #include <stdio.h>
        #include <axiom.h>
        #include <axis2_util.h>
        #include <axiom_soap.h>
        #include <axis2_client.h>

        #ifdef __cplusplus
        extern "C"
        {
        #endif

        #define ADB_DEFAULT_DIGIT_LIMIT 64
        #define ADB_DEFAULT_NAMESPACE_PREFIX_LIMIT 64
        

        /******************************* Create and Free functions *********************************/

        /**
         * Constructor for creating adb_createReservationResponse3_t
         * @param env pointer to environment struct
         * @return newly created adb_createReservationResponse3_t object
         */
        adb_createReservationResponse3_t* AXIS2_CALL
        adb_createReservationResponse3_create(
            const axutil_env_t *env );

        /**
         * Free adb_createReservationResponse3_t object
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_createReservationResponse3_free (
            adb_createReservationResponse3_t* _createReservationResponse3,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        
        

        /**
         * Getter for createReservationResponse. 
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @return adb_createReservationResponse_t*
         */
        adb_createReservationResponse_t* AXIS2_CALL
        adb_createReservationResponse3_get_createReservationResponse(
            adb_createReservationResponse3_t* _createReservationResponse3,
            const axutil_env_t *env);

        /**
         * Setter for createReservationResponse.
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @param arg_createReservationResponse adb_createReservationResponse_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_createReservationResponse3_set_createReservationResponse(
            adb_createReservationResponse3_t* _createReservationResponse3,
            const axutil_env_t *env,
            adb_createReservationResponse_t*  arg_createReservationResponse);

        /**
         * Resetter for createReservationResponse
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_createReservationResponse3_reset_createReservationResponse(
            adb_createReservationResponse3_t* _createReservationResponse3,
            const axutil_env_t *env);

        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether createReservationResponse is nill
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_createReservationResponse3_is_createReservationResponse_nil(
                adb_createReservationResponse3_t* _createReservationResponse3,
                const axutil_env_t *env);


        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_createReservationResponse3_deserialize(
            adb_createReservationResponse3_t* _createReservationResponse3,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_createReservationResponse3_declare_parent_namespaces(
                    adb_createReservationResponse3_t* _createReservationResponse3,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _createReservationResponse3 adb_createReservationResponse3_t object
         * @param env pointer to environment struct
         * @param createReservationResponse3_om_node node to serialize from
         * @param createReservationResponse3_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_createReservationResponse3_serialize(
            adb_createReservationResponse3_t* _createReservationResponse3,
            const axutil_env_t *env,
            axiom_node_t* createReservationResponse3_om_node, axiom_element_t *createReservationResponse3_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_createReservationResponse3 is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_createReservationResponse3_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_CREATERESERVATIONRESPONSE3_H */
    

