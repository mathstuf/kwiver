/*ckwg +29
 * Copyright 2011-2013 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SPROKIT_PIPELINE_PROCESS_H
#define SPROKIT_PIPELINE_PROCESS_H

#include "pipeline-config.h"

#include "edge.h"
#include "config.h"
#include "datum.h"
#include "types.h"

#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>
#include <boost/rational.hpp>
#include <boost/scoped_ptr.hpp>

#include <set>
#include <string>
#include <utility>
#include <vector>

/**
 * \file process.h
 *
 * \brief Header for \link sprokit::process processes\endlink.
 */

namespace sprokit
{

/// A group of processes.
typedef std::vector<process_t> processes_t;

/**
 * \class process process.h <sprokit/pipeline/process.h>
 *
 * \brief A node within a \ref pipeline which runs computations on data.
 *
 * This class represents base class for all processes in the pipeline.
 * Take a look at \ref how_to_process for more detailed explanation on
 * how to write a process.
 *
 * \oports
 *
 * \oport{_heartbeat} Carries the status of the process.
 *
 * \section initialization Initialization Routine
 *
 * <ol>
 *   <li>Configuration is given to the process when constructed.</li>
 *   <li>Connections are made.</li>
 *   <li>The \ref process::_init() method is called.</li>
 * </ol>
 *
 * Exceptions for misconfiguration should be thrown from \ref process::_init()
 * reimplementations. This is to facilitate querying of processes.
 *
 * \section destruction Destruction Routine
 *
 * <ol>
 *   <li>The destructor is called.</li>
 * </ol>
 *
 * Processes should be ready for destruction at any time.
 *
 * \ingroup base_classes
 */
class SPROKIT_PIPELINE_EXPORT process
  : boost::noncopyable
{
  public:
    /// The type for the type of a process.
    typedef std::string type_t;
    /// A group of types.
    typedef std::vector<type_t> types_t;
    /// The type for the name of a process.
    typedef std::string name_t;
    /// The type for a group of process names.
    typedef std::vector<name_t> names_t;
    /// The type for a property on a process.
    typedef std::string property_t;
    /// The type for a set of properties on a process.
    ///@todo Add reference to predefined properties.
    typedef std::set<property_t> properties_t;
    /// The type for a description of a port.
    typedef std::string port_description_t;
    /// The type for the name of a port on a process.
    typedef std::string port_t;
    /// The type for a group of ports.
    typedef std::vector<port_t> ports_t;
    /// The type for the type of data on a port.
    typedef std::string port_type_t;
    /// The type for the component of a frequency.
    typedef size_t frequency_component_t;

    /// \brief The type for the frequency of data on a port. Since
    /// this is a rational type, the frequency can be more or less
    /// than one. Frequencies above one should be integers (e.g. 1/1,
    /// 3/1). Frequencies less than one should have a numerator of one
    /// (e.g. 1/3, 1/5). Frequencies that are not of the above form,
    /// but do resolve to one of those forms are not supported
    /// (e.g. 3/3, 2/4. 4/2).
    typedef boost::rational<frequency_component_t> port_frequency_t;

    /// The type for a flag on a port.
    ///@todo Add reference to predefined port flags.
    typedef std::string port_flag_t;
    /// The type for a group of port flags.
    typedef std::set<port_flag_t> port_flags_t;
    /// The type for the address of a port within the pipeline.
    typedef std::pair<name_t, port_t> port_addr_t;
    /// The type for a group of port addresses.
    typedef std::vector<port_addr_t> port_addrs_t;
    /// The type for a connection within the pipeline.
    typedef std::pair<port_addr_t, port_addr_t> connection_t;
    /// The type for a group of connections.
    typedef std::vector<connection_t> connections_t;

    /**
     * \class port_info process.h <sprokit/pipeline/process.h>
     *
     * \brief Information about a port.
     */
    class SPROKIT_PIPELINE_EXPORT port_info
    {
      public:
        /**
         * \brief Constructor.
         *
         * \param type_ The type of the port.
         * \param flags_ Flags for the port.
         * \param description_ A description of the port.
         * \param frequency_ The frequency of the port relative to the
         * step.  See process::set_output_port_frequency() or
         * process::set_input_port_frequency() for details
         */
        port_info(port_type_t const& type_,
                  port_flags_t const& flags_,
                  port_description_t const& description_,
                  port_frequency_t const& frequency_);
        /**
         * \brief Destructor.
         */
        ~port_info();

        /// The type of the port.
        port_type_t const type;
        /// Flags for the port.
        port_flags_t const flags;
        /// A description of the port.
        port_description_t const description;
        /// The port's frequency.
        port_frequency_t const frequency;
    };
    /// Type for information about a port.
    typedef boost::shared_ptr<port_info const> port_info_t;

    /**
     * \class conf_info process.h <sprokit/pipeline/process.h>
     *
     * \brief Information about a configuration parameter.
     */
    class SPROKIT_PIPELINE_EXPORT conf_info
    {
      public:
        /**
         * \brief Constructor.
         *
         * \param def_ The default value for the parameter.
         * \param description_ A description of the value.
         * \param tunable_ Whether the parameter is tunable or not.
         */
        conf_info(config::value_t const& def_,
                  config::description_t const& description_,
                  bool tunable_);
        /**
         * \brief Destructor.
         */
        ~conf_info();

        /// The default value for the parameter.
        config::value_t const def;
        /// A description of the value.
        config::description_t const description;
        /// Whether the parameter is tunable or not.
        bool const tunable;
    };
    /// Type for information about a configuration parameter.
    typedef boost::shared_ptr<conf_info const> conf_info_t;

    /**
     * \class data_info process.h <sprokit/pipeline/process.h>
     *
     * \brief Information about a set of data.
     */
    class SPROKIT_PIPELINE_EXPORT data_info
    {
      public:
        /**
         * \brief Constructor.
         *
         * \param in_sync_ Whether the data is synchonized.
         * \param max_status_ The highest priority status of the data.
         */
        data_info(bool in_sync_,
                  datum::type_t max_status_);
        /**
         * \brief Destructor.
         */
        ~data_info();

        /// True if the data is synchonized.
        bool const in_sync;
        /// The highest priority status in the set.
        datum::type_t const max_status;
    };
    /// Type for information about a set of data.
    typedef boost::shared_ptr<data_info const> data_info_t;

    /**
     * \brief Data checking levels. All levels include lower levels.
     *
     * \note This is only exposed for easier access from bindings.
     *
     * All levels include lower levels.
     * \sa set_data_checking_level
     */
    typedef enum
    {
      /// Check nothing about incoming data.
      check_none,
      /// Check to ensure incoming data is synchronized.
      check_sync,
      /// Check to ensure incoming data is valid.
      check_valid
    } data_check_t;

    /**
     * \brief Pre-connection initialization.
     *
     * \throw reconfigured_exception Thrown if called multiple times.
     *
     * \postconds
     *
     * \postcond{\c this is ready to be initialized}
     *
     * \endpostconds
     */
    void configure();

    /**
     * \brief Post-connection initialization.
     *
     * \throw unconfigured_exception Thrown if called before \ref configure.
     * \throw reinitialization_exception Thrown if called multiple times.
     *
     * \postconds
     *
     * \postcond{\c this is ready to be stepped}
     *
     * \endpostconds
     */
    void init();

    /**
     * \brief Reset the process.
     *
     * Calling this removes all edges from the process.
     */
    void reset();

    /**
     * \brief Step through one iteration of the process.
     *
     * \preconds
     *
     * \precond{\c this was initialized}
     *
     * \endpreconds
     *
     * \throw unconfigured_exception Thrown if called before \ref configure.
     * \throw uninitialized_exception Thrown if called before \ref init.
     */
    void step();

    /**
     * \brief Query for the properties on the process.
     *
     * \return The set of properties on the process.
     */
    virtual properties_t properties() const;

    /**
     * \brief Connect an edge to an input port on the process.
     *
     * \preconds
     *
     * \precond{\p edge}
     * \precond{The input port \p port exists}
     * \precond{The input port \p port has not been connected to before}
     *
     * \endpreconds
     *
     * \throw null_edge_port_connection_exception Thrown when \p edge is \c NULL.
     * \throw connect_to_initialized_process_exception Thrown if called after \ref init.
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     *
     * \postconds
     *
     * \postcond{The input port \p port is connected via the edge \p edge}
     *
     * \endpostconds
     *
     * \param port The port to connect to.
     * \param edge The edge to connect to the port.
     */
    void connect_input_port(port_t const& port, edge_t edge);
    /**
     * \brief Connect an edge to an output port on the process.
     *
     * \preconds
     *
     * \precond{\p edge}
     * \precond{The output port \p port exists}
     *
     * \endpreconds
     *
     * \throw null_edge_port_connection_exception Thrown when \p edge is \c NULL.
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     *
     * \postconds
     *
     * \postcond{The input port \p port is connected via the edge \p edge}
     *
     * \endpostconds
     *
     * \param port The port to connect to.
     * \param edge The edge to connect to the port.
     */
    void connect_output_port(port_t const& port, edge_t edge);

    /**
     * \brief Query for a list of input ports available on the process.
     *
     * \return The names of all input ports available.
     */
    ports_t input_ports() const;
    /**
     * \brief Query for a list of output ports available on the process.
     *
     * \return The names of all output ports available.
     */
    ports_t output_ports() const;

    /**
     * \brief Query for information about an input port on the process.
     *
     * \warning The returned pointer is not updated if the information for a
     * port changes.
     *
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     *
     * \param port The port to return information about.
     *
     * \return Information about the input port.
     */
    port_info_t input_port_info(port_t const& port);
    /**
     * \brief Query for information about an output port on the process.
     *
     * \warning The returned pointer is not updated if the information for a
     * port changes.
     *
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     *
     * \param port The port to return information about.
     *
     * \return Information about the output port.
     */
    port_info_t output_port_info(port_t const& port);

    /**
     * \brief Set the type of a flow-dependent input port type.
     *
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     * \throw static_type_reset_exception Thrown when the \p port's current type is not dependent on other types.
     * \throw set_type_on_initialized_process_exception Thrown when the \p port's type is set after initialization.
     *
     * \param port The name of the port.
     * \param new_type The type of the port.
     *
     * \return True if the type can work, false otherwise.
     */
    bool set_input_port_type(port_t const& port, port_type_t const& new_type);

    /**
     * \brief Set the type of a flow-dependent output port type.
     *
     * \throw no_such_port_exception Thrown when \p port does not
     * exist on the process.  \throw static_type_reset_exception
     * Thrown when the \p port's current type is not dependent on
     * other types.
     *
     * \throw set_type_on_initialized_process_exception Thrown when
     * the port type is set after initialization.
     *
     * \param port The name of the port.
     * \param new_type The type of the port.
     *
     * \return True if the type can work, false otherwise.
     */
    bool set_output_port_type(port_t const& port, port_type_t const& new_type);

    /**
     * \brief Request available configuration options for the process.
     *
     * \return The names of all available configuration keys.
     */
    config::keys_t available_config() const;

    /**
     * \brief Request available tunable configuration options for the process.
     *
     * \return The names of all available tunable configuration keys.
     */
    config::keys_t available_tunable_config();

    /**
     * \brief Retrieve information about a configuration parameter.
     *
     * \throw unknown_configuration_value_exception Thrown when \p key is not a valid configuration key.
     *
     * \param key The name of the configuration value.
     *
     * \return Information about the parameter.
     */
    conf_info_t config_info(config::key_t const& key);

    /**
     * \brief The name of the process.
     *
     * \return The name of the process.
     */
    name_t name() const;

    /**
     * \brief The type of the process.
     *
     * \return A name for the type of the process.
     */
    type_t type() const;

    /// A property which indicates that the process cannot be run in a thread of its own.
    static property_t const property_no_threads;
    /// A property which indicates that the process is not reentrant.
    static property_t const property_no_reentrancy;
    /// A property which indicates that the input of the process is not synchronized.
    static property_t const property_unsync_input;
    /// A property which indicates that the output of the process is not synchronized.
    static property_t const property_unsync_output;

    /// The name of the heartbeat port.
    static port_t const port_heartbeat;

    /// The name of the configuration value for the name.
    static config::key_t const config_name;
    /// The name of the configuration value for the type.
    static config::key_t const config_type;

    /// A type which means that the type of the data is irrelevant.
    static port_type_t const type_any;
    /// A type which indicates that no actual data is ever created.
    static port_type_t const type_none;

    /**
     * \brief A type which indicates that the type is dependent on data.
     *
     * The process can determine the type, but it must be configured
     * before the type can be pinned down. The is usually for
     * processes which read data from a file which it may not know
     * about until after the configuration has been read.
     */
    static port_type_t const type_data_dependent;

    /**
     *  \brief A type which indicates that the type depends on the
     *  connected port's type.
     *
     * This flag is used when the process wants this port to be typed
     * based on the type of the port that is connected. This can be
     * used when data is just passing through a process and the actual
     * type is not known.
     *
     *  If a tag is appended to the type, then when any of the ports
     *  that use this type name gets a type set for it, all of the
     *  similarly tagged ports are given the same type.
     *
     * Example:
     * \code
      process::port_t const port_input = port_t("pass");

      declare_input_port(
         priv::port_input, // port name
         type_flow_dependent + "tag",
         required,
         port_description_t("The datum to pass."));
     * \endcode
     */
    static port_type_t const type_flow_dependent;

    /**
     * \brief A flag which indicates that the output cannot be modified.
     *
     * Marks that an output is "const" and may not be modified by
     * receivers of the data.
     */
    static port_flag_t const flag_output_const;

    /**
     * \brief A flag which indicates that the output is shared between
     * receivers.
     *
     */
    static port_flag_t const flag_output_shared;

    /**
     * \brief A flag which indicates that the input may be defined as
     * a configuration value.
     *
     * If this port is not connected, the value supplied is taken from
     * a specific config entry. The config entry is automatically
     * named with the key "static/port_name". For example, if the
     * port with this flag is named "foo", then the config entry for
     * the process will be called "static/foo".
     *
     * You can supply a specific default value in the config for the
     * process with the following entry
     *
     * \code
     process circ
       :: circle_writer
          :static/foo  3.14159
     * \endcode
     *
     * This flag may not be combined with \ref flag_required.
     *
     * If the port is connected, the value is passed over the edge and
     * the static config value is not used.
     */
    static port_flag_t const flag_input_static;

    /**
     * \brief A flag which indicates that the input may be modified.
     *
     * Marks that an input is modified within the process and that
     * other receivers of the data may see the changes if the data is
     * not handled carefully.
     */
    static port_flag_t const flag_input_mutable;

    /**
     * \brief A flag which indicates that a connection to the port
     * does not imply a dependency.
     *
     * Indicates that the port is expected to be a backwards edge
     * within the pipeline so that when the pipeline is topologically
     * sorted (either for initialization or execution order by a
     * scheduler), the edge can be ignored for such purposes.
     */
    static port_flag_t const flag_input_nodep;

    /**
     * \brief A flag which indicates that the port is required to be
     * connected.
     *
     */
    static port_flag_t const flag_required;


  protected:
    /**
     * \brief Constructor.
     *
     * \warning Configuration errors must \em not throw exceptions here.
     *
     * \param config Contains configuration for the process.
     */
    process(config_t const& config);
    /**
     * \brief Destructor.
     */
    virtual ~process();

    /**
     * \brief Pre-connection initialization for subclasses.
     *
     * Configuration is where a process is asked to ensure that its
     * configuration makes sense. Any data-dependent port types must
     * be set in this step. After this is called, the process will be
     * have connections made and initialized.
     */
    virtual void _configure();

    /**
     * \brief Post-connection initialization for subclasses.
     *
     * Initialization is the final step before a process is
     * stepped. This is where processes should have a chance to get a
     * first look at the edges that are connected to a port and change
     * behavior based on them. After this is called, the process will
     * be stepped until it is either complete or the scheduler is
     * stopped.
     */
    virtual void _init();

    /**
     * \brief Reset logic for subclasses.
     */
    virtual void _reset();

    /**
     * \brief Flush logic for subclasses.
     */
    virtual void _flush();

    /**
     * \brief Method where subclass data processing occurs.
     *
     * In general, a process' _step method will involve:
     *
     * - Obtaining data from the input edges.
     * - Running the algorithm.
     * - Pushing data out via the output edges.
     *
     */
    virtual void _step();

    /**
     * \brief Runtime configuration for subclasses.
     *
     * This method is called after the process is initially configured
     * and satrted. A config block with updated values is supplied.
     *
     * \params conf The configuration block to apply.
     */
    virtual void _reconfigure(config_t const& conf);

    /**
     * \brief Subclass property query method.
     *
     * \return Properties on the subclass.
     */
    virtual properties_t _properties() const;

    /**
     * \brief Subclass input ports.
     *
     * \return The names of all input ports available in the subclass.
     */
    virtual ports_t _input_ports() const;
    /**
     * \brief Subclass output ports.
     *
     * \return The names of all output ports available in the subclass.
     */
    virtual ports_t _output_ports() const;

    /**
     * \brief Subclass input port information.
     *
     * \param port The port to get information about.
     *
     * \return Information about an output port.
     */
    virtual port_info_t _input_port_info(port_t const& port);
    /**
     * \brief Subclass output port information.
     *
     * \param port The port to get information about.
     *
     * \return Information about an output port.
     */
    virtual port_info_t _output_port_info(port_t const& port);

    /**
     * \brief Subclass input port type setting.
     *
     * \param port The name of the port.
     * \param new_type The type of the connected port.
     *
     * \return True if the type can work, false otherwise.
     */
    virtual bool _set_input_port_type(port_t const& port, port_type_t const& new_type);
    /**
     * \brief Subclass output port type setting.
     *
     * \param port The name of the port.
     * \param new_type The type of the connected port.
     *
     * \return True if the type can work, false otherwise.
     */
    virtual bool _set_output_port_type(port_t const& port, port_type_t const& new_type);

    /**
     * \brief Subclass available configuration keys.
     *
     * \return The names of all available configuration keys.
     */
    virtual config::keys_t _available_config() const;

    /**
     * \brief Subclass configuration information.
     *
     * \param key The name of the configuration value.
     *
     * \return Information about the parameter.
     */
    virtual conf_info_t _config_info(config::key_t const& key);

    /**
     * \brief Declare an input port for the process.
     *
     * \throw null_input_port_info_exception Thrown if \p info is \c NULL.
     *
     * \param port The port name.
     * \param info Information about the port.
     */
    void declare_input_port(port_t const& port, port_info_t const& info);
    /**
     * \brief Declare an output port for the process.
     *
     * \throw null_output_port_info_exception Thrown if \p info is \c NULL.
     *
     * \param port The port name.
     * \param info Information about the port.
     */
    void declare_output_port(port_t const& port, port_info_t const& info);

    /**
     * \brief Declare an input port for the process.
     *
     * \param port The port name.
     * \param type_ The type of the port.
     * \param flags_ Flags for the port.
     * \param description_ A description of the port.
     * \param frequency_ The frequency of the port relative to the
     * step. See process::set_output_port_frequency() for details.
     */
    void declare_input_port(port_t const& port,
                            port_type_t const& type_,
                            port_flags_t const& flags_,
                            port_description_t const& description_,
                            port_frequency_t const& frequency_ = port_frequency_t(1));
    /**
     * \brief Declare an output port for the process.
     *
     * \param port The port name.
     * \param type_ The type of the port.
     * \param flags_ Flags for the port.
     * \param description_ A description of the port.
     * \param frequency_ The frequency of the port relative to the step.
     * See process::set_output_port_frequency() for details.
     */
    void declare_output_port(port_t const& port,
                             port_type_t const& type_,
                             port_flags_t const& flags_,
                             port_description_t const& description_,
                             port_frequency_t const& frequency_ = port_frequency_t(1));

    /**
     * \brief Set the frequency of an input port.
     *
     * This method assigns a frequency to the input port. The number
     * specifies how many inputs are to be accumulated between process
     * step() calls. A frequency of one (the default) will give one
     * input on the port for each step() call. So requesting a
     * frequency of 4 will give the step() method 4 values in the
     * queue for this input.
     *
     * A frequency of zero is a special case.
     *
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     * \throw set_frequency_on_initialized_process_exception Thrown
     * when the \p port's frequency is set after initialization.
     *
     * \param port The name of the port.
     * \param new_frequency The frequency of the port, in the range 1..n
     */
    void set_input_port_frequency(port_t const& port, port_frequency_t const& new_frequency);

    /**
     * \brief Set the frequency of an output port.
     *
     * This method assigns a frequency to the output port. The number
     * specifies how many outputs are pushed downstream between
     * process step() calls. A frequency of one (the default) will
     * produce one output on the port for each step() call. So
     * requesting a frequency of 4 will push 4 values downstream after
     * the step() call queue for this input.
     *
     * A frequency of zero is a special case.
     *
     * \throw no_such_port_exception Thrown when \p port does not exist on the process.
     * \throw set_frequency_on_initialized_process_exception Thrown
     * when the \p port's frequency is set after initialization.
     *
     * \param port The name of the port.
     * \param new_frequency The frequency of the port.
     */
    void set_output_port_frequency(port_t const& port, port_frequency_t const& new_frequency);

    /**
     * \brief Remove an input port from the process.
     *
     * \param port The input port to remove.
     */
    void remove_input_port(port_t const& port);

    /**
     * \brief Remove an output port from the process.
     *
     * \param port The output port to remove.
     */
    void remove_output_port(port_t const& port);

    /**
     * \brief Declare a configuration value for the process.
     *
     * \throw null_conf_info_exception Thrown if \p info is \c NULL.
     *
     * \param key The configuration key.
     * \param info Information about the port.
     */
    void declare_configuration_key(config::key_t const& key, conf_info_t const& info);

    /**
     * \brief Declare a configuration value for the process.
     *
     * \param key The configuration key.
     * \param def_ The default value for the parameter.
     * \param description_ A description of the value.
     * \param tunable_ Whether the parameter is tunable or not.
     */
    void declare_configuration_key(config::key_t const& key,
                                   config::value_t const& def_,
                                   config::description_t const& description_,
                                   bool tunable_ = false);

    /**
     * \brief Mark the process as complete.
     *
     * Calling this method within the step_() method indicates that
     * the process has determined that it should not be called any
     * more and that it is not going to produce any more data.
     *
     * It is considered good form to push a \c datum::complete_datum()
     * element onto each output port.
     *
     * Example:
     *
     * \code
     if (d->fin.eof()) // end of input detected
     {
       mark_process_as_complete();
       dat = datum::complete_datum();
     }
     else
     {
       dat = datum::new_datum(path);
     }

     push_datum_to_port(priv::port_output, dat);
     * \endcode
     */
    void mark_process_as_complete();

    /**
     * \brief Determine if there is an edge connected to an input port.
     *
     * This method is used to determine if the specified port is
     * actually connected to something.
     *
     * \param port The port to get the edge for.
     *
     * \return \b True if there is an edge connected to the \p port,
     * or false if there is none.
     */
    bool has_input_port_edge(port_t const& port) const;

    /**
     * \brief Get the number of connected edges for an output port.
     *
     * \param port The port to get the count for.
     *
     * \return The number of edges connected to the \p port.
     */
    size_t count_output_port_edges(port_t const& port) const;

    /**
     * \brief Peek at an edge datum packet from a port.
     *
     * \param port The port to look at.
     * \param idx The element within the queue to look at.
     *
     * \returns The datum available on the port.
     */
    edge_datum_t peek_at_port(port_t const& port, size_t idx = 0) const;
    /**
     * \brief Peek at a datum packet from a port.
     *
     * \param port The port to look at.
     * \param idx The element within the queue to look at.
     *
     * \returns The datum available on the port.
     */
    datum_t peek_at_datum_on_port(port_t const& port, size_t idx = 0) const;
    /**
     * \brief Grab an edge datum packet from a port.
     *
     *
     * \param port The port to get data from.
     *
     * \return The datum available on the port.
     */
    edge_datum_t grab_from_port(port_t const& port) const;

    /**
     * \brief Grab a datum packet from a port.
     *
     * The datum packet contains the port data and other metadata.
     * See \ref datum for details.
     *
     * \param port The port to get data from.
     *
     * \return The datum available on the port.
     */
    datum_t grab_datum_from_port(port_t const& port) const;

    /**
     * \brief Grab a datum from a port as a certain type.
     *
     * This method grabs an input value directly from the port with no
     * handling for static ports. The returned data value is undefined
     * if there is no input available on the port.
     *
     * \param port The port to get data from.
     *
     * \return The datum from the port.
     */
    template <typename T>
    T grab_from_port_as(port_t const& port) const;

    /**
     * \brief Grab an input as a certain type.
     *
     * This method returns a data value form a port or the configured
     * static value. If there is a value on the port, then this method
     * behaves the same as grab_from_port_as().
     *
     * If there is no value at the port, then the value taken from the
     * configuration entry "static/" + port_name is used.
     *
     * If the templated data type does not have a conversion from a
     * string to an instance of the data type, you will need to
     * provide one in the form of  an input operator.
     \code
     std::istream& operator>> ( std::istream& str, <type>& obj );
     \endcode
     *
     * \param port The port to get data from.
     *
     * \return The input datum.
     */
    template <typename T>
    T grab_input_as(port_t const& port) const;

    /**
     * \brief Output an edge datum packet on a port.
     *
     * \param port The port to push to.
     * \param dat The edge datum to push.
     */
    void push_to_port(port_t const& port, edge_datum_t const& dat) const;

    /**
     * \brief Output a datum packet on a port.
     *
     * \param port The port to push to.
     * \param dat The datum to push.
     */
    void push_datum_to_port(port_t const& port, datum_t const& dat) const;

    /**
     * \brief Output a result on a port.
     * \todo explain why use this instead of push_to_port
     *
     * \param port The port to push to.
     * \param dat The result to push.
     */
    template <typename T>
    void push_to_port_as(port_t const& port, T const& dat) const;

    /**
     * \brief The configuration for the process.
     *
     * \return The whole configuration for the process.
     */
    config_t get_config() const;

    /**
     * \brief Retrieve a configuration key
     *
     * This method returns the configuration value associated with the
     * specified key.  The return value is typed based on the template
     * parameter.
     *
     * \throw no_such_configuration_key_exception Thrown if \p key
     * was not declared for the process.
     *
     * \param key The key to request for the value.
     *
     * \return The value of the configuration.
     */
    template <typename T>
    T config_value(config::key_t const& key) const;

    /**
     * \brief Set whether synchronization checking is enabled before stepping.
     *
     * If set to \ref check_none, no checks on incoming data is performed.
     *
     * If set to \ref check_sync, the input ports which are marked as
     * \flag{required} are guaranteed to be synchronized. When the inputs are
     * not synchronized, an error datum is pushed to all output ports and all
     * input ports will be grabbed from based on the relative frequency of the
     * ports. If this behavior is not wanted, it must be manually handled. The
     * default is that it is enabled.
     *
     * If set to \ref check_valid, the input ports which are marked as
     * \flag{required} are guaranteed to have valid data available. When the
     * inputs are not available, a default corresponding datum packet is
     * generated and pushed to all of the output edges and all input edges will
     * be grabbed from. This implies the \ref check_sync behavior as well.
     *
     * The default is \ref check_valid.
     *
     * \param check The level of validity to check incoming data for.
     */
    void set_data_checking_level(data_check_t check);

    /**
     * \brief Check a set of edge data for certain properties.
     *
     * \param data The data to inspect.
     *
     * \return Information about the data given.
     */
    static data_info_t edge_data_info(edge_data_t const& data);
  private:
    config::value_t config_value_raw(config::key_t const& key) const;

    bool is_static_input(port_t const& port) const;
    static config::key_t const static_input_prefix;

    friend class pipeline;
    SPROKIT_PIPELINE_NO_EXPORT void set_core_frequency(port_frequency_t const& frequency);
    SPROKIT_PIPELINE_NO_EXPORT void reconfigure(config_t const& conf);

    friend class process_cluster;
    SPROKIT_PIPELINE_NO_EXPORT void reconfigure_with_provides(config_t const& conf);

    class SPROKIT_PIPELINE_NO_EXPORT priv;
    boost::scoped_ptr<priv> d;
};

template <typename T>
T
process
::config_value(config::key_t const& key) const
{
  return config_cast<T>(config_value_raw(key));
}

template <typename T>
T
process
::grab_from_port_as(port_t const& port) const
{
  return grab_datum_from_port(port)->get_datum<T>();
}

template <typename T>
T
process
::grab_input_as(port_t const& port) const
{
  if (is_static_input(port) && !has_input_port_edge(port))
  {
    return config_value<T>(static_input_prefix + port);
  }

  return grab_from_port_as<T>(port);
}

template <typename T>
void
process
::push_to_port_as(port_t const& port, T const& dat) const
{
  push_datum_to_port(port, datum::new_datum(dat));
}

}

#endif // SPROKIT_PIPELINE_PROCESS_H
