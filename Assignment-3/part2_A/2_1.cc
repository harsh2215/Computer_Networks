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

using namespace ns3;


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

     std::cout<< "Total throughput of System: "<<
     (totalBytesReceived)/totalTimeReceiving/1024/1024<<" Mbps "<<std::endl;
     std::cout<<"Total packets transmitted: "<<totalPacketsTransmitted<<std::endl;
     std::cout<<"Total packets received: "<< totalPacketsReceived<<std::endl;
     std::cout<<"Total packets dropped: "<< totalPacketsDropped<<std::endl;
     std::cout << " Packet Lost Ratio: " << totalPacketsDropped / (double) (totalPacketsReceived + totalPacketsDropped) << std::endl;
}


int main(int argc, char *argv[]) {
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

    // Print routing tables of routers
    Ptr<OutputStreamWrapper> routingStream1 = Create<OutputStreamWrapper>(&std::cout);
    routers.Get(0)->GetObject<Ipv4>()->GetRoutingProtocol()->PrintRoutingTable(routingStream1);

    Ptr<OutputStreamWrapper> routingStream2 = Create<OutputStreamWrapper>(&std::cout);
    routers.Get(1)->GetObject<Ipv4>()->GetRoutingProtocol()->PrintRoutingTable(routingStream2);

    Ptr<OutputStreamWrapper> routingStream3 = Create<OutputStreamWrapper>(&std::cout);
    routers.Get(2)->GetObject<Ipv4>()->GetRoutingProtocol()->PrintRoutingTable(routingStream3);

    // Generate routing tables file
    std::ofstream routingFile("part_A_routing-tables.txt");	//routing table file
    for (uint32_t i = 0; i < routers.GetN(); ++i) {
        Ptr<Ipv4> ipv4 = routers.Get(i)->GetObject<Ipv4>();
        routingFile << "Router " << i << " Routing Table:\n";
        ipv4->GetRoutingProtocol()->PrintRoutingTable(Create<OutputStreamWrapper>(&routingFile));
        routingFile << "\n\n";
    }

    routingFile.close();


    // Enable tracing
    pointToPoint.EnablePcapAll("A_capture");

    // Create UDP traffic flow
    uint16_t udpPort1 = 53;

    // UDP Flow: From n1 to n5
    Address udpSinkAddress1(InetSocketAddress(centreInterfaces.GetAddress(2), udpPort1));
    PacketSinkHelper udpSinkHelper1("ns3::UdpSocketFactory", udpSinkAddress1);
    ApplicationContainer udpSinkApp1 = udpSinkHelper1.Install(r1_Nodes.Get(2));
    udpSinkApp1.Start(Seconds(1.0));
    udpSinkApp1.Stop(Seconds(10.0));

    OnOffHelper udpClient("ns3::UdpSocketFactory", udpSinkAddress1);
    udpClient.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    udpClient.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    udpClient.SetAttribute("DataRate", DataRateValue(DataRate("5Mbps")));
    udpClient.SetAttribute("PacketSize", UintegerValue(1500));
    ApplicationContainer udpClientApp = udpClient.Install(r0_Nodes.Get(1));
    udpClientApp.Start(Seconds(2.0));
    udpClientApp.Stop(Seconds(10.0));

   // Create TCP traffic flows
    uint16_t Port1 = 8000;
    uint16_t Port2 = 8001;
    uint16_t Port3 = 8002;

    // TCP Flow 1: From n7 to n4
    Address tcpSinkAddress1(InetSocketAddress(centreInterfaces.GetAddress(1), Port1));
    PacketSinkHelper tcpSinkHelper1("ns3::TcpSocketFactory", tcpSinkAddress1);
    ApplicationContainer tcpSinkApp1 = tcpSinkHelper1.Install(r1_Nodes.Get(1));
    tcpSinkApp1.Start(Seconds(1.0));
    tcpSinkApp1.Stop(Seconds(10.0));

    OnOffHelper tcpClient1("ns3::TcpSocketFactory", tcpSinkAddress1);
    tcpClient1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    tcpClient1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    tcpClient1.SetAttribute("DataRate", DataRateValue(DataRate("2Mbps")));
    tcpClient1.SetAttribute("PacketSize", UintegerValue(1000));
    ApplicationContainer tcpClientApp1 = tcpClient1.Install(r2_Nodes.Get(1));
    tcpClientApp1.Start(Seconds(2.0));
    tcpClientApp1.Stop(Seconds(10.0));

    // TCP Flow 2: From n2 to n6
    Address tcpSinkAddress2(InetSocketAddress(endingInterfaces.GetAddress(0), Port2));
    PacketSinkHelper tcpSinkHelper2("ns3::TcpSocketFactory", tcpSinkAddress2);
    ApplicationContainer tcpSinkApp2 = tcpSinkHelper2.Install(r2_Nodes.Get(0));
    tcpSinkApp2.Start(Seconds(1.0));
    tcpSinkApp2.Stop(Seconds(10.0));

    OnOffHelper tcpClient2("ns3::TcpSocketFactory", tcpSinkAddress2);
    tcpClient2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    tcpClient2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    tcpClient2.SetAttribute("DataRate", DataRateValue(DataRate("4Mbps")));
    tcpClient2.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer tcpClientApp2 = tcpClient2.Install(r0_Nodes.Get(2));
    tcpClientApp2.Start(Seconds(2.0));
    tcpClientApp2.Stop(Seconds(10.0));

    // TCP Flow 3: From n0 to n3
    Address tcpSinkAddress3(InetSocketAddress(centreInterfaces.GetAddress(0), Port3));
    PacketSinkHelper tcpSinkHelper3("ns3::TcpSocketFactory", tcpSinkAddress3);
    ApplicationContainer tcpSinkApp3 = tcpSinkHelper3.Install(r1_Nodes.Get(0));
    tcpSinkApp3.Start(Seconds(1.0));
    tcpSinkApp3.Stop(Seconds(10.0));

    OnOffHelper tcpClient3("ns3::TcpSocketFactory", tcpSinkAddress3);
    tcpClient3.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    tcpClient3.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    tcpClient3.SetAttribute("DataRate", DataRateValue(DataRate("1Mbps")));
    tcpClient3.SetAttribute("PacketSize", UintegerValue(728));
    ApplicationContainer tcpClientApp3 = tcpClient3.Install(r0_Nodes.Get(0));
    tcpClientApp3.Start(Seconds(2.0));
    tcpClientApp3.Stop(Seconds(10.0));

    Simulator::Stop (Seconds (20.0)); 

    // Set up NetAnim
    AnimationInterface anim("part_A.xml");	//netanim animation file

    // Create FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    // Run the simulation
    Simulator::Run();
    printStats (flowHelper, true);
    
    // Collect throughput statistics
    monitor->SerializeToXmlFile("A_flow-monitor.xml", true, true);	//flowmonitor file
    Simulator::Destroy();

    return 0;
}
