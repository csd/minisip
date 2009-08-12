

        #ifndef ADB_SETSTATUS_H
        #define ADB_SETSTATUS_H

       /**
        * adb_setStatus.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_setStatus class
        */
        typedef struct adb_setStatus adb_setStatus_t;

        
          #include "adb_statusType.h"
          

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
         * Constructor for creating adb_setStatus_t
         * @param env pointer to environment struct
         * @return newly created adb_setStatus_t object
         */
        adb_setStatus_t* AXIS2_CALL
        adb_setStatus_create(
            const axutil_env_t *env );

        /**
         * Free adb_setStatus_t object
         * @param  _setStatus adb_setStatus_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_free (
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        
        

        /**
         * Getter for sessionId. 
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_setStatus_get_sessionId(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);

        /**
         * Setter for sessionId.
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @param arg_sessionId axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_set_sessionId(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env,
            const axis2_char_t*  arg_sessionId);

        /**
         * Resetter for sessionId
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_reset_sessionId(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);

        
        

        /**
         * Getter for type. 
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return adb_statusType_t*
         */
        adb_statusType_t* AXIS2_CALL
        adb_setStatus_get_type(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);

        /**
         * Setter for type.
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @param arg_type adb_statusType_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_set_type(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env,
            adb_statusType_t*  arg_type);

        /**
         * Resetter for type
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_reset_type(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);

        
        

        /**
         * Getter for description. 
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_setStatus_get_description(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);

        /**
         * Setter for description.
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @param arg_description axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_set_description(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env,
            const axis2_char_t*  arg_description);

        /**
         * Resetter for description
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_reset_description(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env);

        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether sessionId is nill
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_setStatus_is_sessionId_nil(
                adb_setStatus_t* _setStatus,
                const axutil_env_t *env);


        

        /**
         * Check whether type is nill
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_setStatus_is_type_nil(
                adb_setStatus_t* _setStatus,
                const axutil_env_t *env);


        

        /**
         * Check whether description is nill
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_setStatus_is_description_nil(
                adb_setStatus_t* _setStatus,
                const axutil_env_t *env);


        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_setStatus_deserialize(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_setStatus_declare_parent_namespaces(
                    adb_setStatus_t* _setStatus,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _setStatus adb_setStatus_t object
         * @param env pointer to environment struct
         * @param setStatus_om_node node to serialize from
         * @param setStatus_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_setStatus_serialize(
            adb_setStatus_t* _setStatus,
            const axutil_env_t *env,
            axiom_node_t* setStatus_om_node, axiom_element_t *setStatus_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_setStatus is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_setStatus_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_SETSTATUS_H */
    

