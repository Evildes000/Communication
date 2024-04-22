#include "ns3/core-module.h"
#include "ns3/wifi-mode.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/rectangle.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/ipv4-global-routing-helper.h"



NS_LOG_COMPONENT_DEFINE("MyOwnWifiTopology");

uint32_t nWifi1 = 2;
uint32_t nWifi2 = 2;
uint32_t nAp = 2;

int main(int argc, char* argv[]){


    ns3::CommandLine cmd;
    cmd.AddValue("nWifi1","nummber of staWifiNodes in iss 1",nWifi1);
    cmd.AddValue("nWifi2","nummber of staWifiNodes in iss 2",nWifi2);
    cmd.Parse(argc, argv);

    NS_LOG_INFO("This is my first Wifi topology");
    ns3::Time::SetResolution(ns3::Time::NS);
    ns3::LogComponentEnable("UdpEchoClientApplication",ns3::LOG_LEVEL_ALL);
    ns3::LogComponentEnable("UdpEchoServerApplication",ns3::LOG_LEVEL_ALL);

    ns3::NodeContainer staWifinode1;
    ns3::NodeContainer staWifinode2;
    ns3::NodeContainer p2pNodes;

    staWifinode1.Create(nWifi1);
    staWifinode2.Create(nWifi2);
    p2pNodes.Create(nAp);

    ns3::PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", ns3::StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", ns3::StringValue("2ms"));
    ns3::NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    //difference between WifiNetDevice and NetDeviceContainer

    ns3::NetDeviceContainer staWifiDevices1;
    ns3::NetDeviceContainer staWifiDevices2;
    ns3::NetDeviceContainer apWifiDevice1; //--> p2pDevices.Get(0)
    ns3::NetDeviceContainer apWifiDevice2; //--> p2pDevices.Get(1)
    
    ns3::WifiMacHelper mac1;
    ns3::WifiMacHelper mac2;
    ns3::WifiMacHelper apMac1;
    ns3::WifiMacHelper apMac2;
    ns3::Ssid ssid1("This-is-ssid1");
    ns3::Ssid ssid2("This-is-ssid2");
    mac1.SetType("ns3::StaWifiMac","Ssid",ns3::SsidValue(ssid1),"ActiveProbing",ns3::BooleanValue(false)); 
    mac2.SetType("ns3::StaWifiMac","Ssid",ns3::SsidValue(ssid2),"ActiveProbing",ns3::BooleanValue(false));
    apMac1.SetType("ns3::ApWifiMac","Ssid",ns3::SsidValue(ssid1));
    apMac2.SetType("ns3::ApWifiMac","Ssid",ns3::SsidValue(ssid2));

    ns3::YansWifiChannelHelper channel1 = ns3::YansWifiChannelHelper::Default();
    ns3::YansWifiChannelHelper channel2 = ns3::YansWifiChannelHelper::Default();
    ns3::YansWifiPhyHelper phy1;
    ns3::YansWifiPhyHelper phy2;
    //here is different from tutorial,there is no method Default() in class YansWifiChannelHelper anymore
    phy1.SetChannel(channel1.Create());
    phy2.SetChannel(channel2.Create());

    ns3::WifiHelper wifi;
    staWifiDevices1 = wifi.Install(phy1,mac1,staWifinode1);
    staWifiDevices2 = wifi.Install(phy2,mac2,staWifinode2);
    apWifiDevice1 = wifi.Install(phy1,apMac1,p2pNodes.Get(0));
    apWifiDevice2 = wifi.Install(phy2,apMac2,p2pNodes.Get(1));


    ns3::InternetStackHelper stack;
    stack.Install(staWifinode1);
    stack.Install(staWifinode2);
    stack.Install(p2pNodes);

    //so far we have set our nodes,channel,phy and mac. Now we need to assign ip address to nodes.
    //remember to install protocal stack to all nodes before assign ip address to them.
    ns3::Ipv4AddressHelper address;

    //assign ip address to staNode1 in the first iss
    address.SetBase("13.1.1.0","255.255.255.0");
    ns3::Ipv4InterfaceContainer sta1_interface;
    sta1_interface = address.Assign(staWifiDevices1);
    ns3::Ipv4InterfaceContainer ap1_interface;
    ap1_interface = address.Assign(apWifiDevice1);

    //assign ip address to staNode2 in the second iss
    address.SetBase("14.1.1.0","255.255.255.0");
    ns3::Ipv4InterfaceContainer sta2_interface;
    sta2_interface = address.Assign(staWifiDevices2);
    ns3::Ipv4InterfaceContainer ap2_interface;
    ap2_interface = address.Assign(apWifiDevice2);

    //next we will let all staNodes move
    ns3::MobilityHelper mobility1;
    mobility1.SetPositionAllocator(
        "ns3::GridPositionAllocator",
        "MinX", ns3::DoubleValue(0.0),
        "MinY", ns3::DoubleValue(0.0),
        "DeltaX", ns3::DoubleValue(-5.0),
        //distance between tow consecutiv x positons
        "DeltaY", ns3::DoubleValue(10.0),
        "GridWidth", ns3::UintegerValue(3),
        //interval bwtween two consecutiv grid cross points
        "LayoutType", ns3::StringValue("RowFirst")
        //all positions will be allocated at the first row.
        //Then the second row will be allocated "MinY" + "DeltaY"

    );

    mobility1.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                "Bounds", ns3::RectangleValue(ns3::Rectangle(-30,0,-30,30))
        //set boundary to all wifi nodes
    );
    mobility1.Install(staWifinode1);
    //set ap at constant positions
    mobility1.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility1.Install(p2pNodes.Get(0));


    ns3::MobilityHelper mobility2;
    mobility2.SetPositionAllocator(
        "ns3::GridPositionAllocator",
        "MinX", ns3::DoubleValue(0.0),
        "MinY", ns3::DoubleValue(0.0),
        "DeltaX", ns3::DoubleValue(2.0),
        //distance between tow consecutiv x positons
        "DeltaY", ns3::DoubleValue(4.0),
        "GridWidth", ns3::UintegerValue(3),
        //interval bwtween two consecutiv grid cross points
        "LayoutType", ns3::StringValue("RowFirst")
        //all positions will be allocated at the first row.
        //Then the second row will be allocated "MinY" + "DeltaY"

    );

    mobility2.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                "Bounds", ns3::RectangleValue(ns3::Rectangle(0,30,-30,30))
        //set boundary to all wifi nodes
    );

    mobility2.Install(staWifinode2);

    //set two aps at constant positions
    mobility2.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility2.Install(p2pNodes.Get(1));

    

    //following we will set server and client(UDP)
    ns3::UdpEchoServerHelper echoServer(20);
    ns3::ApplicationContainer serverApp = echoServer.Install(staWifinode1.Get(0));
    serverApp.Start(ns3::Seconds(1.0));
    serverApp.Stop(ns3::Seconds(15.0));

    //bind our client in ssb2 to the server
    ns3::UdpEchoClientHelper echoClient(sta1_interface.GetAddress(0), 20);
    echoClient.SetAttribute("MaxPackets",ns3::UintegerValue(1));
    echoClient.SetAttribute("Interval",ns3::TimeValue(ns3::Seconds(1.0)));
    echoClient.SetAttribute("PacketSize",ns3::UintegerValue(1024));


    ns3::ApplicationContainer clientApp = echoClient.Install(staWifinode2.Get(0));
    clientApp.Start(ns3::Seconds(2.0));
    clientApp.Stop(ns3::Seconds(15.0));

    //enable internetwork routing
    ns3::Ipv4GlobalRoutingHelper::RecomputeRoutingTables();

    /*ns3::Simulator::Stop(ns3::Seconds(10.0));
    phy1.EnablePcap("myWifiTopo.cpp", apWifiDevice1.Get(0));
    phy2.EnablePcap("myWifiTopo.cpp", apWifiDevice2.Get(0));
    */

    ns3::Simulator::Stop(ns3::Seconds(20.0));
    ns3::Simulator::Run();
    ns3::Simulator::Destroy();
    return 0;


    //problem: it seems that the server doesn't receive the packet sent from the client.
    //guess to problem: function that called for routing is not correct.

    //task for tonight: figure out what is TypeId originally  
}

