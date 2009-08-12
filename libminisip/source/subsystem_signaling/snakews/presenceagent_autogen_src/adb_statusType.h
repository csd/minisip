

        #ifndef ADB_STATUSTYPE_H
        #define ADB_STATUSTYPE_H

       /**
        * adb_statusType.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_statusType class
        */
        typedef struct adb_statusType adb_statusType_t;

        

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
         * Constructor for creating adb_statusType_t
         * @param env pointer to environment struct
         * @return newly created adb_statusType_t object
         */
        adb_statusType_t* AXIS2_CALL
        adb_statusType_create(
            const axutil_env_t *env );

        /**
         * Free adb_statusType_t object
         * @param  _statusType adb_statusType_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_statusType_free (
            adb_statusType_t* _statusType,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        
        

        /**
         * Getter for statusType. 
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_statusType_get_statusType(
            adb_statusType_t* _statusType,
            const axutil_env_t *env);

        /**
         * Setter for statusType.
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @param arg_statusType axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_statusType_set_statusType(
            adb_statusType_t* _statusType,
            const axutil_env_t *env,
            const axis2_char_t*  arg_statusType);

        /**
         * Resetter for statusType
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_statusType_reset_statusType(
            adb_statusType_t* _statusType,
            const axutil_env_t *env);

        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether statusType is nill
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_statusType_is_statusType_nil(
                adb_statusType_t* _statusType,
                const axutil_env_t *env);


        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
            /**
             * Deserialize the content from a string to adb objects
             * @param  _statusType adb_statusType_t object
             * @param env pointer to environment struct
             * @param node_value to deserialize
             * @param parent_element The parent element if it is an element, NULL otherwise
             * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
             */
            axis2_status_t AXIS2_CALL
            adb_statusType_deserialize_from_string(
                            adb_statusType_t* _statusType,
                                            const axutil_env_t *env,
                                            axis2_char_t *node_value,
                                            axiom_node_t *parent);
        
        /**
         * Deserialize an XML to adb objects
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_statusType_deserialize(
            adb_statusType_t* _statusType,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_statusType_declare_parent_namespaces(
                    adb_statusType_t* _statusType,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        
        /**
         * Serialize to a String from the adb objects
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @param namespaces hash of namespace uri to prefix
         * @return serialized string
         */
            axis2_char_t* AXIS2_CALL
            adb_statusType_serialize_to_string(
                    adb_statusType_t* _statusType,
                    const axutil_env_t *env, axutil_hash_t *namespaces);
        

        /**
         * Serialize to an XML from the adb objects
         * @param  _statusType adb_statusType_t object
         * @param env pointer to environment struct
         * @param statusType_om_node node to serialize from
         * @param statusType_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_statusType_serialize(
            adb_statusType_t* _statusType,
            const axutil_env_t *env,
            axiom_node_t* statusType_om_node, axiom_element_t *statusType_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_statusType is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_statusType_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_STATUSTYPE_H */
    

