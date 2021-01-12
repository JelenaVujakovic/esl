`ifndef SCRAMBLER_IP_SCOREBOARD_SV
`define SCRAMBLER_IP_SCOREBOARD_SV

`uvm_analysis_imp_decl(_axi_lite)
`uvm_analysis_imp_decl(_bram_a)
`uvm_analysis_imp_decl(_bram_b)


class scrambler_ip_scoreboard extends uvm_scoreboard;
	`uvm_component_utils(scrambler_ip_scoreboard)

   scrambler_ip_top_cfg m_cfg;
  //clone items
  axi_lite_item axi_clone_item;
  bram_a_item bram_a_clone;
  bram_b_item bram_b_clone;
  
  int signed bram_a_que[$];
  int unsigned bram_b_que[$];
  int signed data_a;
  int unsigned data_b;
  int number_of_transactions;
 

  uvm_analysis_imp_axi_lite#(axi_lite_item, scrambler_ip_scoreboard) m_axi_lite;
  uvm_analysis_imp_bram_a#(bram_a_item, scrambler_ip_scoreboard) m_bram_a;
  uvm_analysis_imp_bram_b#(bram_b_item, scrambler_ip_scoreboard) m_bram_b;
  
	
  function new(string name = "scrambler_ip_scoreboard", uvm_component parent);
    super.new(name,parent);
  endfunction

	
   extern virtual function void build_phase(uvm_phase phase);	
   extern virtual function void write_axi_lite(axi_lite_item m_axi_item);
   extern virtual function void write_bram_a(bram_a_item m_bram_a_item);
   extern virtual function void write_bram_b(bram_b_item m_bram_b_item);
   extern virtual function void report_phase(uvm_phase phase);

endclass;


function void scrambler_ip_scoreboard::build_phase(uvm_phase phase);
	super.build_phase(phase);
	m_axi_lite = new("m_axi_lite",this);
	m_bram_a = new("m_bram_a",this);
        m_bram_b = new("m_bram_b",this);


   // get configuration
  if(!uvm_config_db #(scrambler_ip_top_cfg)::get(this, "", "m_cfg", m_cfg)) begin
    `uvm_fatal(get_type_name(), "Failed to get configuration object from config DB!")
  end

endfunction: build_phase

function void scrambler_ip_scoreboard::write_axi_lite(axi_lite_item m_axi_item);

   $cast(axi_clone_item,m_axi_item.clone());	


   if(axi_clone_item.addr == READY_REGISTER) begin
    //Check reset value
    if(RESET_REGISTER == 1 && axi_clone_item.rw_op == read) begin
        if(axi_clone_item.data !== 1) 
            `uvm_error(get_type_name(), $sformatf("Reset value of READY should be 1, but it is %d.",axi_clone_item.data))
         else
            `uvm_info(get_type_name(), "Reset value for READY is 1.", UVM_LOW)
     end
    end
  

   else if(axi_clone_item.addr == START_REGISTER) begin
    //Check reset value
    if(RESET_REGISTER == 1 && axi_clone_item.rw_op == read) begin
        if(axi_clone_item.data !== 0) begin
            `uvm_error(get_type_name(), $sformatf("Reset value of START should be 0, but it is %d.",axi_clone_item.data))
        end
        else begin 
            `uvm_info(get_type_name(), "Reset value for START is 0.", UVM_LOW)
        end
        end
    end

  
  if(axi_clone_item.addr == READY_REGISTER || axi_clone_item.addr == RESET_REGISTER || axi_clone_item.addr == START_REGISTER) begin
    `uvm_info(get_type_name(), $sformatf("AXI DATA SCOREBOARD: \n%s", axi_clone_item.sprint()), UVM_DEBUG)
  end
  else begin
   `uvm_error(get_type_name(), $sformatf("Register with the address of %d doesn't exist.",axi_clone_item.addr))
  end

  //ispisivanje item-a za debug
  `uvm_info(get_type_name(), $sformatf("AXI DATA SCOREBOARD: \n%s", axi_clone_item.sprint()), UVM_DEBUG)

endfunction:write_axi_lite

function void scrambler_ip_scoreboard::write_bram_a(bram_a_item m_bram_a_item);

   $cast(bram_a_clone,m_bram_a_item.clone());
   bram_a_que.push_back(bram_a_clone.m_input_data[number_of_transactions]); 
   //Count transactions
   number_of_transactions++; 

    //Check if m_ena signal is set to 1
    asrt_m_ena : assert (bram_a_clone.m_ena == 1)
    `uvm_info(get_type_name(), "Check succesfull: m_ena asserted", UVM_HIGH)
   else
    `uvm_error(get_type_name(), $sformatf("Observed m_ena signal mismatch: m_ena = %0d", bram_a_clone.m_ena))
   //Check if number of transactions is equal to BLOCK SIZE
    asrt_number_of_transactionss_a : assert (number_of_transactions == BLOCK_SIZE)
    `uvm_info(get_type_name(), "Check succesfull: number of transactions is equal to BLOCK SIZE ", UVM_HIGH)
   else
    `uvm_error(get_type_name(), $sformatf("Observed m_ena signal mismatch: number of transactions = %0d", number_of_transactions))
   

  //info za debug
  `uvm_info(get_type_name(), $sformatf("BRAM A SCOREBOARD: \n%s num: %d", bram_a_clone.sprint(), number_of_transactions), UVM_DEBUG)
  `uvm_info(get_type_name(), $sformatf("BRAM_A QUE: \n%p", bram_a_que), UVM_LOW)
	
endfunction: write_bram_a

function void scrambler_ip_scoreboard::write_bram_b(bram_b_item m_bram_b_item);

    $cast(bram_b_clone,m_bram_b_item.clone());
	bram_b_que.push_back(bram_b_clone.m_data_b_out);
  
    /*ako nisam primila start, a dobila sam rezultat -> error
    if(start_happend == 0) begin
        `uvm_error(get_type_name, "Scrambler operation executed but wasn't started")
    end
    if(ready !== 1) begin
        `uvm_error(get_type_name(), "Ready wasn't at 1. Previous scramblerolution was still in progress.")
    end*/
   //Check if m_enb signal is set to 1
    asrt_m_ena : assert (bram_b_clone.m_enb == 1)
    `uvm_info(get_type_name(), "Check succesfull: m_enb asserted", UVM_HIGH)
   else
    `uvm_error(get_type_name(), $sformatf("Observed m_enb signal mismatch: m_enb = %0d", bram_b_clone.m_enb))

    //debug
	`uvm_info(get_type_name(), $sformatf("BRAM B SCOREBOARD: \n%s", bram_b_clone.sprint()), UVM_DEBUG)

   //Address checking
  if(4*order_of_data_b == bram_b_clone.m_addr_b_out) begin
    `uvm_info(get_type_name(),"Address of B is okay !", UVM_LOW)
  end
  else begin
    `uvm_error(get_type_name(),$sformatf("Address of B is %d, it should be %d", bram_b_clone.m_addr_b_out, 4*order_of_data_b))
  end
  order_of_data_b++;

  // Check if data written to bram a and read from bram b match
  for(int i=0; i<BLOCK_SIZE;i++) begin
   data_a = bram_a_que.pop_front();
   data_b = bram_b_que.pop_front(); 
   
   asrt_data_a_eq_data_b : assert (data_a == data_b)
    `uvm_info(get_type_name(), "Check succesfull: data_a == data_b", UVM_HIGH)
   else
    `uvm_error(get_type_name(), $sformatf("Observed data_a and data_b mismatch: data_a = %0d, data_b = %0d", data_a, data_b))
  end	
  

endfunction: write_bram_b

function void scrambler_ip_scoreboard::report_phase(uvm_phase phase);
    `uvm_info(get_type_name(), $sformatf("Scoreboard Report: \n", this.sprint()), UVM_LOW);
endfunction : report_phase

`endif 
