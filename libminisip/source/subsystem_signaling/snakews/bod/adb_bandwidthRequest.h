

        #ifndef ADB_BANDWIDTHREQUEST_H
        #define ADB_BANDWIDTHREQUEST_H

       /**
        * adb_bandwidthRequest.h
        *
        * This file was auto-generated from WSDL
        * by the Apache Axis2/Java version: 1.4.1  Built on : Aug 13, 2008 (05:03:41 LKT)
        */

       /**
        *  adb_bandwidthRequest class
        */
        typedef struct adb_bandwidthRequest adb_bandwidthRequest_t;

        
            #include <axutil_date_time.h>
          

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
         * Constructor for creating adb_bandwidthRequest_t
         * @param env pointer to environment struct
         * @return newly created adb_bandwidthRequest_t object
         */
        adb_bandwidthRequest_t* AXIS2_CALL
        adb_bandwidthRequest_create(
            const axutil_env_t *env );

        /**
         * Free adb_bandwidthRequest_t object
         * @param  _bandwidthRequest adb_bandwidthRequest_t object to free
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_free (
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);



        /********************************** Getters and Setters **************************************/
        /******** Deprecated for array types, Use 'Getters and Setters for Arrays' instead ***********/
        

        /**
         * Getter for bandwidth. 
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return int
         */
        int AXIS2_CALL
        adb_bandwidthRequest_get_bandwidth(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for bandwidth.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_bandwidth int
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_bandwidth(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            const int  arg_bandwidth);

        /**
         * Resetter for bandwidth
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_bandwidth(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        

        /**
         * Getter for destinationIP. Deprecated for array types, Use adb_bandwidthRequest_get_destinationIP_at instead
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return Array of axis2_char_t*s.
         */
        axutil_array_list_t* AXIS2_CALL
        adb_bandwidthRequest_get_destinationIP(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for destinationIP.Deprecated for array types, Use adb_bandwidthRequest_set_destinationIP_at
         * or adb_bandwidthRequest_add_destinationIP instead.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_destinationIP Array of axis2_char_t*s.
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_destinationIP(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            axutil_array_list_t*  arg_destinationIP);

        /**
         * Resetter for destinationIP
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_destinationIP(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        

        /**
         * Getter for destinationSipAddress. 
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_bandwidthRequest_get_destinationSipAddress(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for destinationSipAddress.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_destinationSipAddress axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_destinationSipAddress(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            const axis2_char_t*  arg_destinationSipAddress);

        /**
         * Resetter for destinationSipAddress
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_destinationSipAddress(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        

        /**
         * Getter for end. 
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return axutil_date_time_t*
         */
        axutil_date_time_t* AXIS2_CALL
        adb_bandwidthRequest_get_end(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for end.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_end axutil_date_time_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_end(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            axutil_date_time_t*  arg_end);

        /**
         * Resetter for end
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_end(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        

        /**
         * Getter for sourceIP. Deprecated for array types, Use adb_bandwidthRequest_get_sourceIP_at instead
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return Array of axis2_char_t*s.
         */
        axutil_array_list_t* AXIS2_CALL
        adb_bandwidthRequest_get_sourceIP(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for sourceIP.Deprecated for array types, Use adb_bandwidthRequest_set_sourceIP_at
         * or adb_bandwidthRequest_add_sourceIP instead.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_sourceIP Array of axis2_char_t*s.
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_sourceIP(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            axutil_array_list_t*  arg_sourceIP);

        /**
         * Resetter for sourceIP
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_sourceIP(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        

        /**
         * Getter for sourceSipAddress. 
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return axis2_char_t*
         */
        axis2_char_t* AXIS2_CALL
        adb_bandwidthRequest_get_sourceSipAddress(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for sourceSipAddress.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_sourceSipAddress axis2_char_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_sourceSipAddress(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            const axis2_char_t*  arg_sourceSipAddress);

        /**
         * Resetter for sourceSipAddress
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_sourceSipAddress(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        

        /**
         * Getter for start. 
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return axutil_date_time_t*
         */
        axutil_date_time_t* AXIS2_CALL
        adb_bandwidthRequest_get_start(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        /**
         * Setter for start.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_start axutil_date_time_t*
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_start(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            axutil_date_time_t*  arg_start);

        /**
         * Resetter for start
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_reset_start(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env);

        
        /****************************** Getters and Setters For Arrays **********************************/
        /************ Array Specific Operations: get_at, set_at, add, remove_at, sizeof *****************/

        /**
         * E.g. use of get_at, set_at, add and sizeof
         *
         * for(i = 0; i < adb_element_sizeof_property(adb_object, env); i ++ )
         * {
         *     // Getting ith value to property_object variable
         *     property_object = adb_element_get_property_at(adb_object, env, i);
         *
         *     // Setting ith value from property_object variable
         *     adb_element_set_property_at(adb_object, env, i, property_object);
         *
         *     // Appending the value to the end of the array from property_object variable
         *     adb_element_add_property(adb_object, env, property_object);
         *
         *     // Removing the ith value from an array
         *     adb_element_remove_property_at(adb_object, env, i);
         *     
         * }
         *
         */

        
        
        /**
         * Get the ith element of destinationIP.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @return ith axis2_char_t* of the array
         */
        axis2_char_t* AXIS2_CALL
        adb_bandwidthRequest_get_destinationIP_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i);

        /**
         * Set the ith element of destinationIP. (If the ith already exist, it will be replaced)
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @param arg_destinationIP element to set axis2_char_t* to the array
         * @return ith axis2_char_t* of the array
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_destinationIP_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i,
                const axis2_char_t* arg_destinationIP);


        /**
         * Add to destinationIP.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_destinationIP element to add axis2_char_t* to the array
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_add_destinationIP(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env,
                const axis2_char_t* arg_destinationIP);

        /**
         * Get the size of the destinationIP array.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct.
         * @return the size of the destinationIP array.
         */
        int AXIS2_CALL
        adb_bandwidthRequest_sizeof_destinationIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env);

        /**
         * Remove the ith element of destinationIP.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param i index of the item to remove
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_remove_destinationIP_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i);

        
        
        /**
         * Get the ith element of sourceIP.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @return ith axis2_char_t* of the array
         */
        axis2_char_t* AXIS2_CALL
        adb_bandwidthRequest_get_sourceIP_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i);

        /**
         * Set the ith element of sourceIP. (If the ith already exist, it will be replaced)
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param i index of the item to return
         * @param arg_sourceIP element to set axis2_char_t* to the array
         * @return ith axis2_char_t* of the array
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_sourceIP_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i,
                const axis2_char_t* arg_sourceIP);


        /**
         * Add to sourceIP.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param arg_sourceIP element to add axis2_char_t* to the array
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_add_sourceIP(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env,
                const axis2_char_t* arg_sourceIP);

        /**
         * Get the size of the sourceIP array.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct.
         * @return the size of the sourceIP array.
         */
        int AXIS2_CALL
        adb_bandwidthRequest_sizeof_sourceIP(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env);

        /**
         * Remove the ith element of sourceIP.
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param i index of the item to remove
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_remove_sourceIP_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i);

        


        /******************************* Checking and Setting NIL values *********************************/
        /* Use 'Checking and Setting NIL values for Arrays' to check and set nil for individual elements */

        /**
         * NOTE: set_nil is only available for nillable properties
         */

        

        /**
         * Check whether bandwidth is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_bandwidth_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        

        /**
         * Check whether destinationIP is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_destinationIP_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        
        /**
         * Set destinationIP to nill (currently the same as reset)
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_destinationIP_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);
        

        /**
         * Check whether destinationSipAddress is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_destinationSipAddress_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        

        /**
         * Check whether end is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_end_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        

        /**
         * Check whether sourceIP is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_sourceIP_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        
        /**
         * Set sourceIP to nill (currently the same as reset)
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_sourceIP_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);
        

        /**
         * Check whether sourceSipAddress is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_sourceSipAddress_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        

        /**
         * Check whether start is nill
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_start_nil(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env);


        
        /*************************** Checking and Setting 'NIL' values in Arrays *****************************/

        /**
         * NOTE: You may set this to remove specific elements in the array
         *       But you can not remove elements, if the specific property is declared to be non-nillable or sizeof(array) < minOccurs
         */
        
        /**
         * Check whether destinationIP is nill at i
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct.
         * @param i index of the item to return.
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_destinationIP_nil_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i);
 
       
        /**
         * Set destinationIP to nill at i
         * @param  _bandwidthRequest _ adb_bandwidthRequest_t object
         * @param env pointer to environment struct.
         * @param i index of the item to set.
         * @return AXIS2_SUCCESS on success, or AXIS2_FAILURE otherwise.
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_destinationIP_nil_at(
                adb_bandwidthRequest_t* _bandwidthRequest, 
                const axutil_env_t *env, int i);

        
        /**
         * Check whether sourceIP is nill at i
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct.
         * @param i index of the item to return.
         * @return AXIS2_TRUE if the element is nil or AXIS2_FALSE otherwise
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_sourceIP_nil_at(
                adb_bandwidthRequest_t* _bandwidthRequest,
                const axutil_env_t *env, int i);
 
       
        /**
         * Set sourceIP to nill at i
         * @param  _bandwidthRequest _ adb_bandwidthRequest_t object
         * @param env pointer to environment struct.
         * @param i index of the item to set.
         * @return AXIS2_SUCCESS on success, or AXIS2_FAILURE otherwise.
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_set_sourceIP_nil_at(
                adb_bandwidthRequest_t* _bandwidthRequest, 
                const axutil_env_t *env, int i);

        

        /**************************** Serialize and Deserialize functions ***************************/
        /*********** These functions are for use only inside the generated code *********************/

        
        /**
         * Deserialize an XML to adb objects
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param dp_parent double pointer to the parent node to deserialize
         * @param dp_is_early_node_valid double pointer to a flag (is_early_node_valid?)
         * @param dont_care_minoccurs Dont set errors on validating minoccurs, 
         *              (Parent will order this in a case of choice)
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axis2_status_t AXIS2_CALL
        adb_bandwidthRequest_deserialize(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            axiom_node_t** dp_parent,
            axis2_bool_t *dp_is_early_node_valid,
            axis2_bool_t dont_care_minoccurs);
                            
            

       /**
         * Declare namespace in the most parent node 
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param parent_element parent element
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index pointer to an int which contain the next namespace index
         */
       void AXIS2_CALL
       adb_bandwidthRequest_declare_parent_namespaces(
                    adb_bandwidthRequest_t* _bandwidthRequest,
                    const axutil_env_t *env, axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index);

        

        /**
         * Serialize to an XML from the adb objects
         * @param  _bandwidthRequest adb_bandwidthRequest_t object
         * @param env pointer to environment struct
         * @param bandwidthRequest_om_node node to serialize from
         * @param bandwidthRequest_om_element parent element to serialize from
         * @param tag_closed whether the parent tag is closed or not
         * @param namespaces hash of namespace uri to prefix
         * @param next_ns_index an int which contain the next namespace index
         * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
         */
        axiom_node_t* AXIS2_CALL
        adb_bandwidthRequest_serialize(
            adb_bandwidthRequest_t* _bandwidthRequest,
            const axutil_env_t *env,
            axiom_node_t* bandwidthRequest_om_node, axiom_element_t *bandwidthRequest_om_element, int tag_closed, axutil_hash_t *namespaces, int *next_ns_index);

        /**
         * Check whether the adb_bandwidthRequest is a particle class (E.g. group, inner sequence)
         * @return whether this is a particle class.
         */
        axis2_bool_t AXIS2_CALL
        adb_bandwidthRequest_is_particle();


     #ifdef __cplusplus
     }
     #endif

     #endif /* ADB_BANDWIDTHREQUEST_H */
    

