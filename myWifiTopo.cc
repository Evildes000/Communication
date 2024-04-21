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

uint32_t nWifi1 = 4;
uint32_t nWifi2 = 3;
uint32_t nAp = 2;

int main(){
    ns3::NodeContainer staWifinode1;
    ns3::NodeContainer staWifinode2;
    ns3::NodeContainer p2pNodes;
    ns3::NodeContainer apWifinode1;
    ns3::NodeContainer apWifinode2;
    staWifinode1.Create(nWifi1);
    staWifinode2.Create(nWifi2);
    p2pNodes.Create(nAp);
    apWifinode1 = p2pNodes.Get(0);
    apWifinode2 = p2pNodes.Get(1); 


    ns3::NetDeviceContainer staWifiDevices1;
    ns3::NetDeviceContainer staWifiDevices2;
    ns3::NetDeviceContainer apWifiDevice1;
    ns3::NetDeviceContainer apWifiDevice2;
    
    ns3::WifiMacHelper mac1;
    ns3::WifiMacHelper mac2;
    ns3::Ssid ssid1("This-is-ssid1");
    ns3::Ssid ssid2("This-is-iisd2");
    mac1.SetType("ns3::staWifiMac1","Ssid1",ns3::SsidValue(ssid1),"ActiveProbing"); 
    mac2.SetType("ns3::staWifiMac2","Ssid2",ns3::SsidValue(ssid2),"ActiveProbing");

    ns3::YansWifiChannelHelper channel1 = ns3::YansWifiChannelHelper::Default();
    ns3::YansWifiChannelHelper channel2 = ns3::YansWifiChannelHelper::Default();
    ns3::YansWifiPhyHelper phy1;
    ns3::YansWifiPhyHelper phy2;
    //here is different from tutorial,there is no method Default() in class YansWifiChannelHelper anymore
    phy1.SetChannel(channel1.Create());
    phy2.SetChannel(channel2.Create());

    ns3::WifiHelper wifi1;
    ns3::WifiHelper wifi2;
    staWifiDevices1 = wifi1.Install(phy1,mac1,staWifinode1);
    apWifiDevice1 = wifi1.Install(phy1,mac1,apWifinode1);
    staWifiDevices2 = wifi2.Install(phy2,mac2,staWifinode2);
    apWifiDevice2 = wifi2.Install(phy1,mac1,apWifinode2);

    //so far we have set our nodes,channel,phy and mac. Now we need to assign ip address to nodes.


    ns3::Ipv4AddressHelper address;
    //assign ip address to apndoes
    address.SetBase("12.1.1.0","255.255.255.0");
    ns3::Ipv4InterfaceContainer ap1_interface;
    ap1_interface = address.Assign(apWifiDevice1);
    ns3::Ipv4InterfaceContainer ap2_interface;
    ap2_interface = address.Assign(apWifiDevice2);

    //assign ip address to staNode1 in the first iss
    address.SetBase("13.1.1.0","255.255.255.0");
    ns3::Ipv4InterfaceContainer sta1_interface;
    sta1_interface = address.Assign(staWifiDevices1);

    //assign ip address to staNode2 in the second iss
    address.SetBase("14.1.1.0","255.255.255.0");
    ns3::Ipv4InterfaceContainer sta2_interface;
    sta2_interface = address.Assign(staWifiDevices2);

    //next we will let all stanodes move
    ns3::MobilityHelper mobility;
    mobility.SetPositionAllocator(
        "ns3::GridPositionAllocator",
        "MinX", ns3::DoubleValue(0.0),
        "MinY", ns3::DoubleValue(0.0),
        "DeltaX", ns3::DoubleValue(5.0),
        //distance between tow consecutiv x positons
        "DeltaY", ns3::DoubleValue(10.0),
        "GridWidth", ns3::UintegerValue(3),
        //interval bwtween two consecutiv grid cross points
        "LayoutType", ns3::StringValue("RowFirst")
        //all positions will be allocated at the first row.
        //Then the second row will be allocated "MinY" + "DeltaY"

    );

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                "Bounds", ns3::RectangleValue(ns3::Rectangle(-50,50,-50,50))
        //set boundary to all wifi nodes
    );

    mobility.Install(staWifinode1);
    mobility.Install(staWifinode2);

    //set two aps at constant positions
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apWifinode1);
    mobility.Install(apWifinode2);

    //so far we have set our nodes,channel,phy and mac. Now we need to assign ip address to nodes.
    ns3::InternetStackHelper stack;
    stack.Install(staWifinode1);
    stack.Install(staWifinode2);
    stack.Install(apWifinode1);
    stack.Install(apWifinode2);

    //following we will set server and client(UDP)
    ns3::UdpEchoServerHelper echoServer(20);
    ns3::ApplicationContainer serverApp = echoServer.Install(staWifinode1.Get(0));
    serverApp.Start(ns3::Seconds(1.0));
    serverApp.Start(ns3::Seconds(10.0));

    //bind our client in ssb2 to the server
    ns3::UdpEchoClientHelper echoClient(sta1_interface.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets",ns3::UintegerValue(1));
    echoClient.SetAttribute("Interval",ns3::TimeValue(ns3::Seconds(1.0)));
    echoClient.SetAttribute("PacketSize",ns3::UintegerValue(1024));


    ns3::ApplicationContainer clientApp = echoClient.Install(staWifinode2.Get(0));
    clientApp.Start(ns3::Seconds(2.0));
    clientApp.Stop(ns3::Seconds(10.0));

    //enable internetwork routing
    ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    ns3::Simulator::Stop(ns3::Seconds(10.0));
    phy1.EnablePcap("myWifiTopo.cpp", apWifiDevice1.Get(0));
    phy2.EnablePcap("myWifiTopo.cpp", apWifiDevice2.Get(0));

    ns3::Simulator::Run();
    ns3::Simulator::Destroy();
    return 0;
}

