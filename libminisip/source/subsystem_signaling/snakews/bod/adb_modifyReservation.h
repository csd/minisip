

        #ifndef ADB_MODIFYRESERVATION_H
        #define ADB_MODIFYRESERVATION_H

       /**
        * adb_modifyReservation.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_modifyReservation class
        */
        typedef struct adb_modifyReservation adb_modifyReservation_t;

        
          #include "adb_bandwidthRequest.h"
          

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
         * Constructor for creating adb_modifyReservation_t
         * @param env pointer to environment struct
         * @return newly created adb_modifyReservation_t object
         */
        adb_modifyReservation_t* AXIS2_CALL
        adb_modifyReservation_create(
            const axutil_env_t *env );

        /**
         * Free adb_modifyReservation_t object
         * @param  _modifyReservation adb_modifyReservation_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_free (
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        
        

        /**
         * Getter for sessionId. 
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_modifyReservation_get_sessionId(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        /**
         * Setter for sessionId.
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param arg_sessionId axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_set_sessionId(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env,
            const axis2_char_t*  arg_sessionId);

        /**
         * Resetter for sessionId
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_reset_sessionId(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        
        

        /**
         * Getter for reservationId. 
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_modifyReservation_get_reservationId(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        /**
         * Setter for reservationId.
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param arg_reservationId axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_set_reservationId(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env,
            const axis2_char_t*  arg_reservationId);

        /**
         * Resetter for reservationId
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_reset_reservationId(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        
        

        /**
         * Getter for bandwidthRequest. 
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return adb_bandwidthRequest_t*
         */
        adb_bandwidthRequest_t* AXIS2_CALL
        adb_modifyReservation_get_bandwidthRequest(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        /**
         * Setter for bandwidthRequest.
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param arg_bandwidthRequest adb_bandwidthRequest_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_set_bandwidthRequest(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env,
            adb_bandwidthRequest_t*  arg_bandwidthRequest);

        /**
         * Resetter for bandwidthRequest
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_reset_bandwidthRequest(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        
        

        /**
         * Getter for bidirectional. 
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return axis2_bool_t
         */
        axis2_bool_t AXIS2_CALL
        adb_modifyReservation_get_bidirectional(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        /**
         * Setter for bidirectional.
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param arg_bidirectional axis2_bool_t
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_set_bidirectional(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env,
            axis2_bool_t  arg_bidirectional);

        /**
         * Resetter for bidirectional
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_reset_bidirectional(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env);

        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether sessionId is nill
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_modifyReservation_is_sessionId_nil(
                adb_modifyReservation_t* _modifyReservation,
                const axutil_env_t *env);


        

        /**
         * Check whether reservationId is nill
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_modifyReservation_is_reservationId_nil(
                adb_modifyReservation_t* _modifyReservation,
                const axutil_env_t *env);


        

        /**
         * Check whether bandwidthRequest is nill
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_modifyReservation_is_bandwidthRequest_nil(
                adb_modifyReservation_t* _modifyReservation,
                const axutil_env_t *env);


        

        /**
         * Check whether bidirectional is nill
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_modifyReservation_is_bidirectional_nil(
                adb_modifyReservation_t* _modifyReservation,
                const axutil_env_t *env);


        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_modifyReservation_deserialize(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_modifyReservation_declare_parent_namespaces(
                    adb_modifyReservation_t* _modifyReservation,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _modifyReservation adb_modifyReservation_t object
         * @param env pointer to environment struct
         * @param modifyReservation_om_node node to serialize from
         * @param modifyReservation_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_modifyReservation_serialize(
            adb_modifyReservation_t* _modifyReservation,
            const axutil_env_t *env,
            axiom_node_t* modifyReservation_om_node, axiom_element_t *modifyReservation_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_modifyReservation is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_modifyReservation_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_MODIFYRESERVATION_H */
    

