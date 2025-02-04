/*
 * canopen_manager.hpp
 *
 *  Created on: 18.12.2015
 *      Author: feesma44884
 */

#ifndef CANOPEN_MANAGER_HPP_
#define CANOPEN_MANAGER_HPP_

/**
 * @class CANopen::Manager
 *
 * @brief CANopen Manager class manages communication via the CANopen protocol
 */
class CANopen::Manager : public CANopen::Singleton<CANopen::Manager>
{
friend class CANopen::Singleton<CANopen::Manager>;
public:
    enum State  {
        MNS_START               =   0,
        MNS_CONFIG              =   1,
        MNS_PRE_OPERATIONAL     =   2,
        MNS_OPERATIONAL         =   3
    };

    /**
     * @brief Destructor of manager class
     */
    ~Manager(void);

    /**
     * @brief Initializes the CANopen::Manager
     *
     * @param node_handle[in] ROS node handle
     * @param sync_frequency_hz[in] Frequency sync signal is send and RPDO data
     * @param reset[in] Resets all drives
     * @param update_frequency_hz[in] Update frequency of the node
     */
    void init(ros::NodeHandle& node_handle, const double sync_frequency_hz, const bool reset = false, const double update_frequency_hz = 1000.0);

    /**
     * @brief Adds a node to the node list
     *
     * @param node_id[in] CANopen Node-ID of the node
     */
    void add_node(const uint8_t node_id);

    /**
     * @brief Registers a function which will be called after standard configuration is done (like PDO mapping)
     *
     * @param pre_op_config_func[in] Pointer to function
     */
    void register_pre_operational_config_function(bool (*pre_op_config_func)(void));

    /**
     * @brief Sends a CAN message via ROS to the CAN network
     *
     * @param can_msg[in] CAN message
     */
    void    can_tx(const MSG& can_msg);

    /**
     * @brief Get state of the CANopen::Manager
     *
     * @return state of the manager
     */
    const State& get_state(void);

    /**
     * @brief Returns the node with the specified CAN ID
     *
     * @param node_id[in] CANopen Node-ID of the node
     * @return
     */
    std::shared_ptr<Node> const get_node(const uint8_t node_id);

    /**
     * @brief Returns the PDO of the specified node
     *
     * @param node_id[in] CANopen Node-ID of the node
     * @param cob_id[in] PDO COB-ID
     *
     * @return Const pointer to the PDO object
     */
    PDO*  const get_PDO(const uint8_t can_id, const COB& cob_id);

    /**
     * @brief Returns a pointer to the object dictionary
     *
     * @return Pointer to object dictionary
     */
    OD* const get_OD(void);

    /**
     * @brief Returns the reference to the update frequency
     *
     * @return update frequency in Hz
     */
    const double& get_update_frequency_Hz(void);

    /**
     * @brief Enables debug mode (shows all CAN messages)
     */
    void    debug_on(void)      {_debug_mode = true;}

    /**
     * @brief Disables debug mode
     */
    void    debug_off(void)     {_debug_mode = false;}
private:
    /**
     * @brief Constructor of class is private
     */
    Manager(void);

    /**
     * @brief Updates the manager and is called by _update_timer
     *
     * @param event[in] callback value
     */
    void    update(const ros::TimerEvent& event);

    /**
     * @brief Receives CAN messages from the ROS network
     *
     * @param can_data[in] callback value -> received CAN message data
     */
    void    can_rx(const ohm_can_interface::CAN::ConstPtr& can_data);

    /**
     * @brief Starts the can open network
     *
     * @return
     */
    bool    start(void);

    /**
     * @brief Configures all nodes
     *
     * @return true if configuration is ok
     */
    bool    config(void);

    /**
     * @brief Calls user pre op function
     *
     * @return true if everything is ok
     */
    bool    pre_operational(void);

    /**
     * @brief Sends a sync signal to all nodes
     */
    void    SYNC(const ros::TimerEvent& event);

    OD                        _object_dictionary;           //!< Object dictionary
    NMT                       _network_management;          //!< Network management interface
    State                     _manager_state;               //!< State of the manager

    ros::Timer                _update_timer;                //!< Timer for updating or running the manager with desired loop rate
    ros::Timer                _sync_timer;                  //!< Timer for sending SYNC signal
    ros::Subscriber           _can_subscriber;              //!< Receives CAN messages from ROS
    ros::Publisher            _can_publisher;               //!< Transmit CAN messages via ROS

    double                    _update_frequency_hz;         //!< Update frequency of the node in Hz
    double                    _sync_frequency_hz;           //!< Send frequency of SYNC() signal and R-PDO data
    unsigned int              _sub_state;                   //!< Variable used for saving the current state of the sub function called by _manager_state

    std::map<uint8_t, std::shared_ptr<Node>>  _node_list;             //!< List with CANopen-nodes
    uint8_t                                   _num_nodes_registered;  //!< Number of nodes registred through network

    bool                      _enable_operational_mode;     //!< True if operational mode is enabled -> all motor controllers

    bool                      _debug_mode;                  //!< True debug is on | false debug is off
    bool                      _reset;                       //!< Saves true if drive has to be reseted

    bool                      (*_pre_op_user_config)(void); //!< Ptr. to user function for pre operational mode

    friend class CANopen::NMT;
    friend class CANopen::Node;
};

#endif /* CANOPEN_MANAGER_HPP_ */
