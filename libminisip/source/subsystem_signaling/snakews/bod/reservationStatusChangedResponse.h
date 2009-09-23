

        #ifndef ADB_RESERVATIONSTATUSCHANGEDRESPONSE_H
        #define ADB_RESERVATIONSTATUSCHANGEDRESPONSE_H

       /**
        * adb_reservationStatusChangedResponse.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_reservationStatusChangedResponse class
        */
        typedef struct adb_reservationStatusChangedResponse adb_reservationStatusChangedResponse_t;

        

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
         * Constructor for creating adb_reservationStatusChangedResponse_t
         * @param env pointer to environment struct
         * @return newly created adb_reservationStatusChangedResponse_t object
         */
        adb_reservationStatusChangedResponse_t* AXIS2_CALL
        adb_reservationStatusChangedResponse_create(
            const axutil_env_t *env );

        /**
         * Free adb_reservationStatusChangedResponse_t object
         * @param  _reservationStatusChangedResponse adb_reservationStatusChangedResponse_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_reservationStatusChangedResponse_free (
            adb_reservationStatusChangedResponse_t* _reservationStatusChangedResponse,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _reservationStatusChangedResponse adb_reservationStatusChangedResponse_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_reservationStatusChangedResponse_deserialize(
            adb_reservationStatusChangedResponse_t* _reservationStatusChangedResponse,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _reservationStatusChangedResponse adb_reservationStatusChangedResponse_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_reservationStatusChangedResponse_declare_parent_namespaces(
                    adb_reservationStatusChangedResponse_t* _reservationStatusChangedResponse,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _reservationStatusChangedResponse adb_reservationStatusChangedResponse_t object
         * @param env pointer to environment struct
         * @param reservationStatusChangedResponse_om_node node to serialize from
         * @param reservationStatusChangedResponse_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_reservationStatusChangedResponse_serialize(
            adb_reservationStatusChangedResponse_t* _reservationStatusChangedResponse,
            const axutil_env_t *env,
            axiom_node_t* reservationStatusChangedResponse_om_node, axiom_element_t *reservationStatusChangedResponse_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_reservationStatusChangedResponse is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_reservationStatusChangedResponse_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_RESERVATIONSTATUSCHANGEDRESPONSE_H */
    

