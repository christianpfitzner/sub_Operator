/*
 * canopen_node.hpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */

#ifndef CANOPEN_NODE_HPP_
#define CANOPEN_NODE_HPP_

/**
 * @class CANopen::Node
 *
 * @brief Class representing one physical CANopen node
 */
class CANopen::Node
{
public:
    enum State {
        NS_RESET    =   0,
        NS_START    =   1
    };

    /**
     * @brief Constructor
     * @param node_id[in] CANopen Node-ID of the node
     */
    Node(const uint8_t node_id);

    /**
     * @brief Adds a SDO to the send buffer of the node
     *
     * @param access[in]: Read/write access
     * @param object_id[in]: ID of the object in the object dictionary
     * @param multiplexer[in]: Multiplexer or sub-index
     * @param data[in]: data to sent
     * @param response_time_ms[in] time to wait for a response (100 ms default)
     *
     */
    void send_SDO(const SDO::ACCESS_TYPE access, const uint16_t object_id, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms = 100);

    /**
     * @brief Adds a SDO to the send buffer of the node
     *
     * @param access[in]: Read/write access
     * @param object[in]: Reference to an object from OD
     * @param multiplexer[in]: Multiplexer or sub-index
     * @param data[in]: data to sent
     * @param response_time_ms[in] time to wait for a response (100 ms default)
     */
    void send_SDO(const SDO::ACCESS_TYPE, const OD::Object& object, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms = 100);

    /**
     * @brief Adds a SDO to the send buffer of the node
     *
     * @param access[in]: Read/write access
     * @param object_name[in]: Name of the object
     * @param multiplexer[in]: Multiplexer or sub-index
     * @param data[in]: data to sent
     * @param response_time_ms[in] time to wait for a response (100 ms default)
     */
    void send_SDO(const SDO::ACCESS_TYPE, const std::string& object_name, const uint8_t multiplexer, const uint32_t data, const unsigned int response_time_ms = 100);

    /**
     * @brief Adds a timeout to the message que to wait for an event
     *
     * @param timeout_ms[in] Time to wait in milliseconds
     */
    void SDO_sleep(const unsigned int timeout_ms);

    /**
     * @return Returns the CANopen Node-ID of the node
     */
    const uint8_t   get_node_id(void);

    /**
     * @brief Returns the specified PDO
     *
     * @param cob_id[in] COB ID of the PDO
     *
     * @return Reference to the PDO
     */
    PDO* const get_PDO(const COB cob_id);
private:
    /**
     * @brief Updates the nodes and send commands
     */
    void update(void);

    /**
     * @brief SYNC signal is send from manager -> update PDOs
     */
    void SYNC(void);

    /**
     * @brief Processes the received data
     *
     * @param cob_type[in] CANopen Object Type
     * @param can_data[in] Data
     *
     * @return true if everything is ok
     */
    bool process_data(const COB cob_type, const MSG& can_data);

    /**
     * @brief Sends the PDO configuration to the node
     */
    void send_PDO_config(void);

    const uint8_t           _node_id;               //!< NODE-ID of the CANopen node
    State                   _node_state;            //!< State of the node

    std::map<COB, PDO>      _PDO_list;              //!< List with all PDO objects

    std::queue<SDO>         _SDO_queue;             //!< Queue of SDO data to sent
    unsigned int            _SDO_timeout_cnt;       //!< Saves the count which have passed since the message was sent
    unsigned int            _SDO_max_timeout_cnt;   //!< Saves the maximum cnts of send cnt before timeout is thrown

    bool                    _is_registered;         //!< Saves true when node is registered in network

    friend class CANopen::Manager;
};

#endif /* CANOPEN_NODE_HPP_ */
