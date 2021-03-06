`ifndef BRAM_A_AGENT_SV
`define BRAM_A_AGENT_SV

class bram_a_agent extends uvm_agent;
  
  // registration macro
  `uvm_component_utils(bram_a_agent)
  
  // analysis port
  uvm_analysis_port #(bram_a_item) m_aport;
    
  // virtual interface reference
  virtual interface bram_a_if m_vif;
      
  // configuration reference
  bram_a_agent_cfg m_cfg;
  
  // components instances
  bram_a_driver m_driver;
  bram_a_sequencer m_sequencer;
  bram_a_monitor m_monitor;
  bram_a_coverage m_coverage;
  
  // constructor
  extern function new(string name, uvm_component parent);
  // build phase
  extern virtual function void build_phase(uvm_phase phase);
  // connect phase
  extern virtual function void connect_phase(uvm_phase phase);
  // print configuration
  extern virtual function void print_cfg();

endclass : bram_a_agent

// constructor
function bram_a_agent::new(string name, uvm_component parent);
  super.new(name, parent);
endfunction : new

// build phase
function void bram_a_agent::build_phase(uvm_phase phase);
  super.build_phase(phase);
  
  // create ports
  m_aport = new("m_aport", this);
  
  // get interface
  if(!uvm_config_db#(virtual bram_a_if)::get(this, "", "m_vif", m_vif)) begin
    `uvm_fatal(get_type_name(), "Failed to get virtual interface from config DB!")
  end
  
  // get configuration
  if(!uvm_config_db #(bram_a_agent_cfg)::get(this, "", "m_cfg", m_cfg)) begin
    `uvm_fatal(get_type_name(), "Failed to get configuration object from config DB!")
  end else begin
    print_cfg();
  end
    
  // create components
  if (m_cfg.m_is_active == UVM_ACTIVE) begin
    m_driver = bram_a_driver::type_id::create("m_driver", this);
    m_sequencer = bram_a_sequencer::type_id::create("m_sequencer", this);
  end
  m_monitor = bram_a_monitor::type_id::create("m_monitor", this);
  if (m_cfg.m_has_coverage == 1) begin
    m_coverage = bram_a_coverage::type_id::create("m_coverage", this);
  end  
endfunction : build_phase

// connect phase
function void bram_a_agent::connect_phase(uvm_phase phase);
  super.connect_phase(phase);
  
  // connect ports
  if (m_cfg.m_is_active == UVM_ACTIVE) begin
    m_driver.seq_item_port.connect(m_sequencer.seq_item_export);
  end
  m_monitor.m_aport.connect(m_aport);
  if (m_cfg.m_has_coverage == 1) begin
    m_monitor.m_aport.connect(m_coverage.analysis_export);
  end
  
  // assign interface
  if (m_cfg.m_is_active == UVM_ACTIVE) begin
    m_driver.m_vif = m_vif;
  end
  m_monitor.m_vif = m_vif;
  
  // assign configuration
  if (m_cfg.m_is_active == UVM_ACTIVE) begin    
    m_driver.m_cfg = m_cfg;
    m_sequencer.m_cfg = m_cfg;
  end
  m_monitor.m_cfg = m_cfg;
  if (m_cfg.m_has_coverage == 1) begin
    m_coverage.m_cfg = m_cfg;
  end
endfunction : connect_phase

// print configuration
function void bram_a_agent::print_cfg();
  `uvm_info(get_type_name(), $sformatf("Configuration: \n%s", m_cfg.sprint()), UVM_HIGH)
endfunction : print_cfg

`endif // BRAM_A_AGENT_SV
