

        #ifndef ADB_USERSTATUS_H
        #define ADB_USERSTATUS_H

       /**
        * adb_userStatus.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_userStatus class
        */
        typedef struct adb_userStatus adb_userStatus_t;

        
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
         * Constructor for creating adb_userStatus_t
         * @param env pointer to environment struct
         * @return newly created adb_userStatus_t object
         */
        adb_userStatus_t* AXIS2_CALL
        adb_userStatus_create(
            const axutil_env_t *env );

        /**
         * Free adb_userStatus_t object
         * @param  _userStatus adb_userStatus_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_free (
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        
        

        /**
         * Getter for description. 
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_userStatus_get_description(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);

        /**
         * Setter for description.
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @param arg_description axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_set_description(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env,
            const axis2_char_t*  arg_description);

        /**
         * Resetter for description
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_reset_description(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);

        
        

        /**
         * Getter for sipAddress. 
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_userStatus_get_sipAddress(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);

        /**
         * Setter for sipAddress.
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @param arg_sipAddress axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_set_sipAddress(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env,
            const axis2_char_t*  arg_sipAddress);

        /**
         * Resetter for sipAddress
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_reset_sipAddress(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);

        
        

        /**
         * Getter for type. 
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return adb_statusType_t*
         */
        adb_statusType_t* AXIS2_CALL
        adb_userStatus_get_type(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);

        /**
         * Setter for type.
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @param arg_type adb_statusType_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_set_type(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env,
            adb_statusType_t*  arg_type);

        /**
         * Resetter for type
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_reset_type(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env);

        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether description is nill
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_userStatus_is_description_nil(
                adb_userStatus_t* _userStatus,
                const axutil_env_t *env);


        

        /**
         * Check whether sipAddress is nill
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_userStatus_is_sipAddress_nil(
                adb_userStatus_t* _userStatus,
                const axutil_env_t *env);


        

        /**
         * Check whether type is nill
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_userStatus_is_type_nil(
                adb_userStatus_t* _userStatus,
                const axutil_env_t *env);


        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_userStatus_deserialize(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_userStatus_declare_parent_namespaces(
                    adb_userStatus_t* _userStatus,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _userStatus adb_userStatus_t object
         * @param env pointer to environment struct
         * @param userStatus_om_node node to serialize from
         * @param userStatus_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_userStatus_serialize(
            adb_userStatus_t* _userStatus,
            const axutil_env_t *env,
            axiom_node_t* userStatus_om_node, axiom_element_t *userStatus_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_userStatus is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_userStatus_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_USERSTATUS_H */
    

