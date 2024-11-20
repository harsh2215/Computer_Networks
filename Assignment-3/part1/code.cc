#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd;
  int questionNumber;
  cmd.AddValue("question", "Select the question number (1-4)", questionNumber);
  cmd.Parse (argc, argv);

  if (questionNumber == 1)
  {
	    NodeContainer nodes;
	    nodes.Create(2);

	    PointToPointHelper pointToPoint;
	    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	    NetDeviceContainer devices;
	    devices = pointToPoint.Install(nodes);

	    InternetStackHelper stack;
	    stack.Install(nodes);

	    Ipv4AddressHelper address;
	    address.SetBase ("10.1.1.0", "255.255.255.0");
	    Ipv4InterfaceContainer interfaces = address.Assign(devices);

	    AnimationInterface anim ("1.xml");

	    Simulator::Stop (Seconds (10.0));
	    Simulator::Run();
	    Simulator::Destroy();
  }
  else if (questionNumber == 2)
  {
	    NodeContainer nodes;
	    nodes.Create(2);

	    PointToPointHelper pointToPoint;
	    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	    NetDeviceContainer devices;
	    devices = pointToPoint.Install(nodes);

	    InternetStackHelper stack;
	    stack.Install(nodes);

	    Ipv4AddressHelper address;
	    address.SetBase ("10.1.1.0", "255.255.255.0");
	    Ipv4InterfaceContainer interfaces = address.Assign(devices);

	    uint16_t port = 8080;
	    UdpServerHelper server (port);
	    ApplicationContainer serverApp = server.Install (nodes.Get(1));
	    serverApp.Start (Seconds (1.0));
	    serverApp.Stop (Seconds (10.0));

	    UdpClientHelper client (interfaces.GetAddress(1), port);
	    client.SetAttribute ("MaxPackets", UintegerValue (320));
	    client.SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
	    client.SetAttribute ("PacketSize", UintegerValue (1024));

	    ApplicationContainer clientApp = client.Install (nodes.Get(0));
	    clientApp.Start (Seconds (2.0));
	    clientApp.Stop (Seconds (10.0));

	    AnimationInterface anim ("2.xml");

	    Simulator::Stop (Seconds (10.0));
	    Simulator::Run();
	    Simulator::Destroy();
  }
  else if (questionNumber == 3)
  {
	  
	     double throughput = 0.0;

	  // Loop to vary latency
	  for (double delayMs = 500.0; delayMs <= 1500.0; delayMs += 500.0)
	  {
	    std::cout << "Running simulation with delay: " << delayMs << " ms" << std::endl;

	    // Create nodes
	    NodeContainer nodes;
	    nodes.Create(2);

	    // Create point-to-point link with the given delay
	    PointToPointHelper pointToPoint;
	    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	    pointToPoint.SetChannelAttribute ("Delay", StringValue (std::to_string(delayMs) + "ms"));

	    NetDeviceContainer devices;
	    devices = pointToPoint.Install(nodes);

	    // Install internet stack
	    InternetStackHelper stack;
	    stack.Install(nodes);

	    // Assign IP addresses
	    Ipv4AddressHelper address;
	    address.SetBase ("10.1.1.0", "255.255.255.0");
	    Ipv4InterfaceContainer interfaces = address.Assign(devices);

	    // Set up UDP server on Node 1 (receiver)
	    uint16_t port = 8080;
	    UdpServerHelper server (port);
	    ApplicationContainer serverApp = server.Install (nodes.Get(1));
	    serverApp.Start (Seconds (1.0));
	    serverApp.Stop (Seconds (12.0));  // Extend time for longer data transmission

	    // Set up UDP client on Node 0 (sender)
	    UdpClientHelper client (interfaces.GetAddress(1), port);
	    client.SetAttribute ("MaxPackets", UintegerValue (10000)); // More packets to reflect longer transmission
	    client.SetAttribute ("Interval", TimeValue (MilliSeconds (1))); // Adjust packet rate to prevent saturation
	    client.SetAttribute ("PacketSize", UintegerValue (1024)); // 1024 bytes per packet

	    ApplicationContainer clientApp = client.Install (nodes.Get(0));
	    clientApp.Start (Seconds (2.0));
	    clientApp.Stop (Seconds (11.0)); // Match client stop time with server stop time

	    // Enable animation output (optional)
	    AnimationInterface anim ("animation_" + std::to_string(delayMs) + ".xml");

	    // Run the simulation
	    Simulator::Stop (Seconds (12.0));
	    Simulator::Run();

	    // Calculate throughput directly from UdpServer
	    Ptr<UdpServer> udpServer = DynamicCast<UdpServer> (serverApp.Get(0));
	    uint64_t totalBytesReceived = udpServer->GetReceived();
	    
	    // Calculate throughput in Mbps (9 seconds of transmission time)
	    throughput = totalBytesReceived * 8.0 / (9.0 * 1000000);

	    std::cout << "Throughput with delay " << delayMs << " ms: " << throughput << " Mbps" << std::endl;

	    // Clean up after simulation run
	    Simulator::Destroy();
	  }
  }
  else if (questionNumber == 4)
  {
	  NodeContainer nodes;
	  nodes.Create(2);

	  PointToPointHelper pointToPoint;
	  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	  NetDeviceContainer devices = pointToPoint.Install(nodes);

	  InternetStackHelper stack;
	  stack.Install(nodes);

	  Ipv4AddressHelper address;
	  address.SetBase ("10.1.1.0", "255.255.255.0");
	  Ipv4InterfaceContainer interfaces = address.Assign(devices);

	  UdpServerHelper server1 (8080);
	  ApplicationContainer serverApp1 = server1.Install (nodes.Get(1));
	  serverApp1.Start (Seconds (1.0));
	  serverApp1.Stop (Seconds (10.0));

	  UdpClientHelper client1 (interfaces.GetAddress(1), 8080);
	  client1.SetAttribute ("MaxPackets", UintegerValue (320));
	  client1.SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
	  client1.SetAttribute ("PacketSize", UintegerValue (1024));

	  ApplicationContainer clientApp1 = client1.Install (nodes.Get(0));
	  clientApp1.Start (Seconds (2.0));
	  clientApp1.Stop (Seconds (10.0));

	  UdpServerHelper server2 (8081);  // Different port to avoid conflict
	  ApplicationContainer serverApp2 = server2.Install (nodes.Get(1));
	  serverApp2.Start (Seconds (1.0));
	  serverApp2.Stop (Seconds (10.0));

	  UdpClientHelper client2 (interfaces.GetAddress(1), 8081);  // Different port to avoid conflict
	  client2.SetAttribute ("MaxPackets", UintegerValue (320));
	  client2.SetAttribute ("Interval", TimeValue (MilliSeconds (10)));
	  client2.SetAttribute ("PacketSize", UintegerValue (1024));

	  ApplicationContainer clientApp2 = client2.Install (nodes.Get(0));
	  clientApp2.Start (Seconds (2.0));
	  clientApp2.Stop (Seconds (10.0));

	  AnimationInterface anim ("4.xml");

	  Simulator::Stop (Seconds (10.0));

	  Simulator::Run();
	  Simulator::Destroy();

	  return 0;
  }
  else
  {
  	std::cout << "Invalid question number. Please select a number between 1 and 4." << std::endl;
  	std::cout << "Usage: ./ns3 run scratch/your_program.cc --question=<question-number>" << std::endl;
  }
  
  return 0;
}

