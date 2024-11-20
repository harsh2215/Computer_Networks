#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/mobility-module.h"
#include "fstream"
#include "ns3/error-model.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("part_B");

void printStats (FlowMonitorHelper &flowmon_helper, bool perFlowInfo) {
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon_helper.GetClassifier());
  std::string proto;
  Ptr<FlowMonitor> monitor = flowmon_helper.GetMonitor ();
  std::map < FlowId, FlowMonitor::FlowStats > stats = monitor->GetFlowStats();
  double totalTimeReceiving;
  uint64_t totalPacketsReceived, totalPacketsDropped, totalBytesReceived,totalPacketsTransmitted;

  totalBytesReceived = 0, totalPacketsDropped = 0, totalPacketsReceived = 0, totalTimeReceiving = 0,totalPacketsTransmitted = 0;
  for (std::map< FlowId, FlowMonitor::FlowStats>::iterator flow = stats.begin(); flow != stats.end(); flow++)
  {
    Ipv4FlowClassifier::FiveTuple  t = classifier->FindFlow(flow->first);
    switch(t.protocol)
     {
     case(6):
         proto = "TCP";
         break;
     case(17):
         proto = "UDP";
         break;
     default:
         exit(1);
     }
     totalBytesReceived += (double) flow->second.rxBytes * 8;
     totalTimeReceiving += flow->second.timeLastRxPacket.GetSeconds ()-flow->second.timeFirstTxPacket.GetSeconds();
     totalPacketsReceived += flow->second.rxPackets;
     totalPacketsDropped += flow->second.txPackets - flow->second.rxPackets;
     totalPacketsTransmitted += flow->second.txPackets;
     if (perFlowInfo)
     {
       std::cout << "FlowID: " << flow->first << " (" << proto << " "
                 << t.sourceAddress << " / " << t.sourcePort << " --> "
                 << t.destinationAddress << " / " << t.destinationPort << ")" << std::endl;
       std::cout << "  Tx Bytes: " << flow->second.txBytes << std::endl;
       std::cout << "  Rx Bytes: " << flow->second.rxBytes << std::endl;
       std::cout << "  Tx Packets: " << flow->second.txPackets << std::endl;
       std::cout << "  Rx Packets: " << flow->second.rxPackets << std::endl;
       std::cout << "  Time LastRxPacket: " << flow->second.timeLastRxPacket.GetSeconds () << "s" << std::endl;
       std::cout << "  Lost Packets: " << flow->second.lostPackets << std::endl;
       std::cout << "  Pkt Lost Ratio: " << ((double)flow->second.txPackets-(double)flow->second.rxPackets)/(double)flow->second.txPackets << std::endl;
       std::cout << "  Throughput: " << ( ((double)flow->second.rxBytes * 8) / (flow->second.timeLastRxPacket.GetSeconds ()-flow->second.timeFirstTxPacket.GetSeconds())/1024/1024 ) << " Mbps" << std::endl;
       std::cout << "  Mean{Delay}: " << (flow->second.delaySum.GetSeconds()/flow->second.rxPackets) << std::endl;
       std::cout << "  Mean{Jitter}: " << (flow->second.jitterSum.GetSeconds()/(flow->second.rxPackets)) << std::endl;
     }


   }

     std::cout<<"Total throughput of System: "<<(totalBytesReceived)/totalTimeReceiving/1024/1024<<" Mbps "<<std::endl;
     std::cout<<"Total packets transmitted: "<<totalPacketsTransmitted<<std::endl;
     std::cout<<"Total packets received: "<< totalPacketsReceived<<std::endl;
     std::cout<<"Total packets dropped: "<< totalPacketsDropped<<std::endl;
     std::cout<<"Packet Delivery Ratio: "<< totalPacketsReceived / (double) (totalPacketsReceived + totalPacketsDropped) << std::endl;
}


int main(int argc, char *argv[]) {

    uint32_t Rng_seed = 1;	//Default seed value
    double e_Rate = 0.001;	//Default error rate 
    std::string e_Unit = "packet";	//Default error unit
    ns3::RateErrorModel::ErrorUnit errorUnitEnum = ns3::RateErrorModel::ERROR_UNIT_PACKET;
    
    LogComponentEnable("part_B", LOG_LEVEL_ERROR);	//Enable logging

    CommandLine cmd;
    cmd.AddValue("RngRun", "Seed for the random number generator", Rng_seed);
    cmd.AddValue("errorRate", "Error rate of the error model", e_Rate);
    cmd.AddValue("errorUnit", "Error unit of the error model", e_Unit);
    cmd.Parse(argc, argv);

    if (e_Unit == "bit") {        
        errorUnitEnum = ns3::RateErrorModel::ERROR_UNIT_BIT;
    } 
    else if (e_Unit == "byte") {
        errorUnitEnum = ns3::RateErrorModel::ERROR_UNIT_BYTE;
    }
    else if (e_Unit == "packet") {
        errorUnitEnum = ns3::RateErrorModel::ERROR_UNIT_PACKET;
    }
    else {        
        NS_FATAL_ERROR("Invalid error unit specified. Correct values are: bit, byte, packet");        
    }

    
    // Set the random number generator seed value
    RngSeedManager::SetSeed(Rng_seed);
    
    // Set the default time resolution
    Time::SetResolution(Time::NS);

    // Create nodes
	NodeContainer r0_Nodes, r1_Nodes, r2_Nodes, routers;
	r0_Nodes.Create(3);   // n0, n1, n2
	routers.Create(3);    // router0, router1, router2
	r1_Nodes.Create(3);   // n3, n4, n5
	r2_Nodes.Create(2);   // n6, n7
    
    // Set constant positions for all nodes
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> pos = CreateObject<ListPositionAllocator>();

	// Set positions for r0_Nodes
	pos->Add(Vector(10.0, 30.0, 0.0));  // n0
	pos->Add(Vector(10.0, 40.0, 0.0));  // n1
	pos->Add(Vector(10.0, 50.0, 0.0));  // n2

	// Set positions for routers
	pos->Add(Vector(20.0, 40.0, 0.0));  // router0
	pos->Add(Vector(30.0, 20.0, 0.0));  // router1
	pos->Add(Vector(30.0, 55.0, 0.0));  // router2

	// Set positions for r1_Nodes
	pos->Add(Vector(40.0, 10.0, 0.0));  // n3
	pos->Add(Vector(45.0, 20.0, 0.0));  // n4
	pos->Add(Vector(40.0, 30.0, 0.0));  // n5

	// Set positions for r2_Nodes
	pos->Add(Vector(50.0, 50.0, 0.0));  // n6
	pos->Add(Vector(50.0, 60.0, 0.0));  // n7
	
    mobility.SetPositionAllocator(pos);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    // Install mobility model on all nodes
    mobility.Install(r0_Nodes);
    mobility.Install(routers);
    mobility.Install(r1_Nodes);
    mobility.Install(r2_Nodes);

    // Setup Point-to-Point link parameters
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("20Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));
    
    // Install P2P links from r0_Nodes to router0
    NetDeviceContainer startingDevices;
    for (uint32_t i = 0; i < r0_Nodes.GetN(); ++i) {
        NetDeviceContainer link = pointToPoint.Install(r0_Nodes.Get(i), routers.Get(0));
        startingDevices.Add(link);
    }

    // Install P2P links from r1_Nodes to router1
    NetDeviceContainer centreDevices;
    for (uint32_t i = 0; i < r1_Nodes.GetN(); ++i) {
        NetDeviceContainer link = pointToPoint.Install(r1_Nodes.Get(i), routers.Get(1));
        centreDevices.Add(link);
    }

    // Install P2P links from r2_Nodes to router2
    NetDeviceContainer endingDevices;
    for (uint32_t i = 0; i < r2_Nodes.GetN(); ++i) {
        NetDeviceContainer link = pointToPoint.Install(r2_Nodes.Get(i), routers.Get(2));
        endingDevices.Add(link);
    }
    
    // Install P2P links between routers (router0 to router1 and router2)
    PointToPointHelper routerp2p;
    routerp2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    routerp2p.SetChannelAttribute("Delay", StringValue("2ms"));
    
    NetDeviceContainer routerDevices;
    routerDevices.Add(routerp2p.Install(routers.Get(0), routers.Get(1)));
    routerDevices.Add(routerp2p.Install(routers.Get(0), routers.Get(2)));


    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(r0_Nodes);
    stack.Install(routers);
    stack.Install(r1_Nodes);
    stack.Install(r2_Nodes);
    

    // Assign IP addresses
    Ipv4AddressHelper address;
    Ipv4InterfaceContainer startingInterfaces, routerInterfaces, centreInterfaces, endingInterfaces;

    // Assign IP addresses to nodes (n0, n1, n2) connected to router0
    for (uint32_t i = 0; i < 3; ++i) {
        std::ostringstream subnet;
        subnet << "192.168." << i + 1 << ".0";
        address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
        startingInterfaces.Add(address.Assign(startingDevices.Get(i * 2)));
        address.Assign(startingDevices.Get(i * 2 + 1));
    }

    // Assign IP addresses to routers
    for (uint32_t i = 0; i < 2; ++i) {
        std::ostringstream subnet;
        subnet << "192.167." << i + 1 << ".0";
        address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
        routerInterfaces.Add(address.Assign(routerDevices.Get(i*2)));
        address.Assign(routerDevices.Get(i*2+1));
    }

    // Assign IP addresses to nodes (n3, n4, n5) connected to router1
    for (uint32_t i = 0; i < 3; ++i) {
        std::ostringstream subnet;
        subnet << "192.166." << i + 1 << ".0";
        address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
        centreInterfaces.Add(address.Assign(centreDevices.Get(i * 2)));  // Assign IP to node's device
        address.Assign(centreDevices.Get(i * 2 + 1));  // Assign IP to router's device
    }

    // Assign IP addresses to nodes (n6, n7) connected to router2
    for (uint32_t i = 0; i < 2; ++i) {
        std::ostringstream subnet;
        subnet << "192.165." << i + 1 << ".0";
        address.SetBase(Ipv4Address(subnet.str().c_str()), "255.255.255.0");
        endingInterfaces.Add(address.Assign(endingDevices.Get(i * 2)));  // Assign IP to node's device
        address.Assign(endingDevices.Get(i * 2 + 1));  // Assign IP to router's device
    }
    
    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Enable tracing
    pointToPoint.EnablePcapAll("B_capture");


    // Create UdpEchoServer applications From n1 to n5
    UdpEchoServerHelper echoServer(46); 
    ApplicationContainer serverApps = echoServer.Install(r1_Nodes.Get(2));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Create UdpEchoClient applications From n1 to n5
    UdpEchoClientHelper echoClient(centreInterfaces.GetAddress(2), 46);
    echoClient.SetAttribute("MaxPackets", UintegerValue(50));      
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(100.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(r0_Nodes.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));



    // Create UdpEchoServer applications From n7 to n4
    UdpEchoServerHelper echoServer1(47); 
    ApplicationContainer serverApps1 = echoServer1.Install(r1_Nodes.Get(1));
    serverApps1.Start(Seconds(1.0));
    serverApps1.Stop(Seconds(10.0));

    // Create UdpEchoClient applications From n7 to n4
    UdpEchoClientHelper echoClient1(centreInterfaces.GetAddress(1), 47);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(50));       
    echoClient1.SetAttribute("Interval", TimeValue(MilliSeconds(100.0)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps1 = echoClient1.Install(r2_Nodes.Get(1));
    clientApps1.Start(Seconds(2.0));
    clientApps1.Stop(Seconds(10.0));

    

    //Create UdpEchoServer applications From n2 to n6
    UdpEchoServerHelper echoServer2(53);
    ApplicationContainer serverApps2 = echoServer2.Install(r2_Nodes.Get(0));
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(10.0));

    // Create UdpEchoClient applications From n2 to n6
    UdpEchoClientHelper echoClient2(endingInterfaces.GetAddress(0), 53);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(50));       
    echoClient2.SetAttribute("Interval", TimeValue(MilliSeconds(100.0)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps2 = echoClient2.Install(r0_Nodes.Get(2));
    clientApps2.Start(Seconds(2.0));
    clientApps2.Stop(Seconds(10.0));




    // Create UdpEchoServer applications From n0 to n3
    UdpEchoServerHelper echoServer3(54);
    ApplicationContainer serverApps3 = echoServer3.Install(r1_Nodes.Get(0));
    serverApps3.Start(Seconds(1.0));
    serverApps3.Stop(Seconds(10.0));
 
    // Create UdpEchoClient applications From n0 to n3
    UdpEchoClientHelper echoClient3(centreInterfaces.GetAddress(0), 54);
    echoClient3.SetAttribute("MaxPackets", UintegerValue(50));       
    echoClient3.SetAttribute("Interval", TimeValue(MilliSeconds(100.0)));
    echoClient3.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps3 = echoClient3.Install(r0_Nodes.Get(0));
    clientApps3.Start(Seconds(2.0));
    clientApps3.Stop(Seconds(10.0));



    // Define the error model
    Ptr<RateErrorModel> errorModel = CreateObject<RateErrorModel>();
    errorModel->SetAttribute("ErrorRate",DoubleValue(e_Rate));
    errorModel->SetAttribute("ErrorUnit",EnumValue(errorUnitEnum));

    // Installing error model on the links and devices

    for (uint32_t i = 0; i < startingDevices.GetN(); ++i)
    {
        startingDevices.Get(i)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));
    }

    for (uint32_t i = 0; i < centreDevices.GetN(); ++i)
    {
        centreDevices.Get(i)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));
    }

    for (uint32_t i = 0; i < endingDevices.GetN(); ++i)
    {
        endingDevices.Get(i)->SetAttribute("ReceiveErrorModel", PointerValue(errorModel));
    }

    Simulator::Stop (Seconds (30.0)); 

    // Set up NetAnim
    AnimationInterface anim("part_B.xml");	//netanim animation file

    // Create FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

	//Run the Simulation
    Simulator::Run();
    printStats (flowHelper, true);
    
    //Collect Throughput Statistics
    monitor->SerializeToXmlFile("B_flow-monitor.xml", true, true);	//flowmonitor file 

    Simulator::Destroy();

    return 0;
}
