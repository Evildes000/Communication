#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

//include "ns3/tsn-module.h"
#include "ns3/tsn-helper.h"
/*#include "ns3/net-device-list-config.h"
#include "ns3/tas-queue-disc.h"
#include "ns3/transmisson-gate-qdisc.h"
#include "ns3/TsnIpv4PacketFilter.h"
#include "ns3/TsnIpv6PacketFilter.h"
#include "ns3/Ipv4Filter.h"
*/
#include "ns3/traffic-control-module.h"
#include "ns3/perfect-clock-model-impl.h"
#include "ns3/local-clock.h"
#include "ns3/Sta-priority.h"
#include "ns3/bridge-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor.h"
#include "stdio.h"
#include <array>
#include <string>
#include <chrono>


using namespace ns3;

int main(){

    bool verbose = true;
    if(verbose){
        LogComponentEnable("FqCoDelQueueDisc",LOG_LEVEL_FUNCTION);
        LogComponentEnable ("UdpClient", LOG_LEVEL_FUNCTION);
        LogComponentEnable ("UdpServer", LOG_LEVEL_FUNCTION);
    }


    NodeContainer nodes;
    nodes.Create(4);
    NodeContainer theOther;
    theOther.Create(3);
    NodeContainer mySwitch;
    mySwitch.Create(1);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevcies, theOtherCsmaDevices;
    csmaDevcies = csma.Install(nodes);
    theOtherCsmaDevices = csma.Install(theOther);

    BridgeHelper bridge;
    NetDeviceContainer bridge;
    bridgeDevices = bridge.Install(nodes.Get(1), csmaDevcies.Get(1));


   /* TrafficControlHelper tch;
    uint16_t handle = tch.SetRootQueueDisc("PrioQueueDisc");
    tch.Install(csmaDevcies.Get(0));
    tch.Install(csmaDevcies.Get(3));*/

    InternetStackHelper stack;
    stack.Install(nodes);
    stack.Install(theOther);

    Ipv4AddressHelper address;
    address.SetBase("0.0.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces;
    interfaces = address.Assign(csmaDevcies);

    UdpServerHelper server (9);
    ApplicationContainer serverApps = server.Install (nodes.Get (0));
    serverApps.Start (Seconds (0));
    serverApps.Stop (Seconds(10));


    UdpClientHelper client (interfaces.GetAddress (0), 9);

    client.SetAttribute ("MaxPackets", UintegerValue (1000));
    client.SetAttribute ("Interval", TimeValue (Seconds(1)));// 0,25s
    client.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApp = client.Install (nodes.Get (2));


    double ap_init_offset = 0;
    double ap_freq = 1;

    Ptr<PerfectClockModelImpl> clockImpl_ap = CreateObject<PerfectClockModelImpl>();
    clockImpl_ap->SetAttribute("Offset", TimeValue(Seconds(ap_init_offset)));
    clockImpl_ap->SetAttribute("Frequency", DoubleValue(ap_freq));

    Ptr<LocalClock> client_clock = CreateObject<LocalClock>();
    client_clock->SetAttribute("ClockModel",PointerValue(clockImpl_ap));
    nodes.Get(2)->AggregateObject(client_clock);

    Ptr<LocalClock> server_clock = CreateObject<LocalClock>();
    server_clock->SetAttribute("ClockModel",PointerValue(clockImpl_ap));
    nodes.Get(0)->AggregateObject(server_clock);



    clientApp.Start (Seconds(1));
    clientApp.Stop (Seconds(10));

    Simulator::Run();
    Simulator::Stop(Seconds(11));

    Simulator::Destroy();




}

