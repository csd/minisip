

        #ifndef ADB_SNAKEEVENT_H
        #define ADB_SNAKEEVENT_H

       /**
        * adb_snakeEvent.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_snakeEvent class
        */
        typedef struct adb_snakeEvent adb_snakeEvent_t;

        

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
         * Constructor for creating adb_snakeEvent_t
         * @param env pointer to environment struct
         * @return newly created adb_snakeEvent_t object
         */
        adb_snakeEvent_t* AXIS2_CALL
        adb_snakeEvent_create(
            const axutil_env_t *env );

        /**
         * Free adb_snakeEvent_t object
         * @param  _snakeEvent adb_snakeEvent_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_free (
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        
        

        /**
         * Getter for eventId. 
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return int64_t
         */
        int64_t AXIS2_CALL
        adb_snakeEvent_get_eventId(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        /**
         * Setter for eventId.
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param arg_eventId int64_t
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_set_eventId(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env,
            const int64_t  arg_eventId);

        /**
         * Resetter for eventId
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_reset_eventId(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        
        

        /**
         * Getter for methodId. 
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_snakeEvent_get_methodId(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        /**
         * Setter for methodId.
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param arg_methodId axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_set_methodId(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env,
            const axis2_char_t*  arg_methodId);

        /**
         * Resetter for methodId
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_reset_methodId(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        
        

        /**
         * Getter for receiver. 
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_snakeEvent_get_receiver(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        /**
         * Setter for receiver.
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param arg_receiver axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_set_receiver(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env,
            const axis2_char_t*  arg_receiver);

        /**
         * Resetter for receiver
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_reset_receiver(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        
        

        /**
         * Getter for serviceName. 
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_snakeEvent_get_serviceName(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        /**
         * Setter for serviceName.
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param arg_serviceName axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_set_serviceName(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env,
            const axis2_char_t*  arg_serviceName);

        /**
         * Resetter for serviceName
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_reset_serviceName(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env);

        


        /******************************* Checking and Setting NIL values *********************************/
        

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether eventId is nill
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_snakeEvent_is_eventId_nil(
                adb_snakeEvent_t* _snakeEvent,
                const axutil_env_t *env);


        

        /**
         * Check whether methodId is nill
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_snakeEvent_is_methodId_nil(
                adb_snakeEvent_t* _snakeEvent,
                const axutil_env_t *env);


        

        /**
         * Check whether receiver is nill
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_snakeEvent_is_receiver_nil(
                adb_snakeEvent_t* _snakeEvent,
                const axutil_env_t *env);


        

        /**
         * Check whether serviceName is nill
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_snakeEvent_is_serviceName_nil(
                adb_snakeEvent_t* _snakeEvent,
                const axutil_env_t *env);


        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_snakeEvent_deserialize(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_snakeEvent_declare_parent_namespaces(
                    adb_snakeEvent_t* _snakeEvent,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _snakeEvent adb_snakeEvent_t object
         * @param env pointer to environment struct
         * @param snakeEvent_om_node node to serialize from
         * @param snakeEvent_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_snakeEvent_serialize(
            adb_snakeEvent_t* _snakeEvent,
            const axutil_env_t *env,
            axiom_node_t* snakeEvent_om_node, axiom_element_t *snakeEvent_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_snakeEvent is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_snakeEvent_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_SNAKEEVENT_H */
    

