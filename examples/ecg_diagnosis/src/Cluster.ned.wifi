//Interface module/wrapper between OMNET++ MAC/PHY layers and full system simulator 
import NoS.applications.ethernet.CliWrapper;
import NoS.applications.ethernet.SrvWrapper;
import NoS.applications.ethernet.SmartAP;

//Energy model for power calculation
import inet.power.contract.IEnergyStorage;
import inet.power.contract.IEnergyGenerator;
import inet.mobility.single.CircleMobility;
import inet.mobility.static.StationaryMobility;

//Header files of 802.11
import inet.linklayer.ieee80211.Ieee80211Nic;
import inet.mobility.single.CircleMobility;
import inet.mobility.static.StationaryMobility;
import inet.node.wireless.AccessPoint;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;

//Header files of 802.15.4
import inet.linklayer.ieee802154.Ieee802154NarrowbandNic;
import inet.physicallayer.ieee802154.packetlevel.Ieee802154NarrowbandScalarRadioMedium;

//Header files for Access Point
import inet.common.lifecycle.NodeStatus;
import inet.linklayer.contract.IMACAddressTable;
import inet.linklayer.contract.IMACRelayUnit;
import inet.linklayer.contract.IWiredNic;
import inet.linklayer.contract.IWirelessNic;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;
import inet.linklayer.ieee8021d.relay.Ieee8021dRelay;

module SmartAccessPoint
{
    parameters:
        @networkNode();
        @labels(node,ethernet-node,wireless-node);
        @display("i=device/accesspoint");
        string mobilityType = default("StationaryMobility"); // how the node moves around. see: inet.mobility
        string relayUnitType = default(firstAvailable("Ieee8021dRelay","MACRelayUnit")); // type of the IMACRelayUnit (MACRelayUnit, Ieee8021dRelay, etc)
        string macTableType = default("MACAddressTable"); // type of the ~IMACAddressTable
        int numRadios = default(1);               // the number of radios in the access point
        bool hasStatus = default(false);
        wlan[*].mgmtType = default("Ieee80211MgmtAPSimplified");
        eth[*].encapType = "EtherEncapDummy";
        *.interfaceTableModule = default(absPath(".interfaceTable"));
    gates:
        input radioIn[numRadios] @directIn;
        inout ethg[] @labels(EtherFrame-conn);
    submodules:
        status: NodeStatus if hasStatus {
            @display("p=51,43");
        }

        interfaceTable: InterfaceTable {
            @display("p=51,121;is=s");
        }

        mobility: <mobilityType> like IMobility {
            parameters:
                @display("p=208,43");
        }

        macTable: <macTableType> like IMACAddressTable {
            @display("p=73,19;is=s");
        }

        ap: SmartAP {
            parameters:
                @display("p=151,121");
        }

        //ap: Ieee8021dRelay {
        //    parameters:
        //        @display("p=151,121");
        //}
        //srv: SrvWrapper {
        //    parameters:
        //        registerSAP = false;
        //        @display("p=210,68,col");
        //}

        wlan[numRadios]: <default("Ieee80211Nic")> like IWirelessNic {
            parameters:
                @display("p=89,225,row;q=queue");
        }

    connections allowunconnected:
        // connections to network outside
        // wireless interfaces MUST be connected first (i.e. ports 0..numRadios-1)
        // because broadcasts must be handled differently for wireless IFs by the relayUnit
        for i=0..numRadios-1 {
            radioIn[i] --> wlan[i].radioIn;
            wlan[i].upperLayerOut --> ap.ifIn++ ;
            wlan[i].upperLayerIn <-- ap.ifOut++ ;
        }
        // ethernet must be connected only AFTER wireless ports
        for i=0..sizeof(ethg)-1 {
            eth[i].phys <--> ethg[i];
            eth[i].upperLayerIn <-- ap.ifOut++ if (sizeof(ethg)+numRadios)>1;
            eth[i].upperLayerOut --> ap.ifIn++ if (sizeof(ethg)+numRadios)>1;
        }
        //wlan.radioIn <-- radioIn[0];
        //wlan.upperLayerOut --> srv.in;
        //srv.out --> wlan.upperLayerIn;

}

module ClusterClient
{
    parameters:
        @display("i=device/wifilaptop");
        @networkNode();
        *.interfaceTableModule = "";
        string energyStorageType = default("");
        string energyGeneratorType = default("");
        *.energySourceModule = default(energyStorageType != "" ? absPath(".energyStorage") : "");
        //@node();

    gates:
        input radioIn @directIn;

    submodules:
        energyStorage: <energyStorageType> like IEnergyStorage if energyStorageType != "" {
            parameters:
                @display("p=50,100;i=block/plug;is=s");
        }
        energyGenerator: <energyGeneratorType> like IEnergyGenerator if energyGeneratorType != "" {
            parameters:
                @display("p=50,150;i=block/plug;is=s");
        }

        cli: CliWrapper {
            parameters:
                @display("b=40,24;p=180,60,col");
        }

        wlan: Ieee80211Nic {
            parameters:
                @display("p=112,134;q=queue");
                mgmtType = "Ieee80211MgmtSTASimplified";
        }
        mobility: StationaryMobility {
            parameters:
                @display("p=50,141");
        }

    connections allowunconnected:
        wlan.radioIn <-- radioIn;
        cli.out --> wlan.upperLayerIn;
        wlan.upperLayerOut --> cli.in;
}

module ClusterServer
{
    parameters:
        @display("i=device/wifilaptop");
        @networkNode();
        *.interfaceTableModule = "";
        string energyStorageType = default("");
        string energyGeneratorType = default("");
        *.energySourceModule = default(energyStorageType != "" ? absPath(".energyStorage") : "");

        //@node();
    gates:
        input radioIn @directIn;

    submodules:
        energyStorage: <energyStorageType> like IEnergyStorage if energyStorageType != "" {
            parameters:
                @display("p=50,100;i=block/plug;is=s");
        }
        energyGenerator: <energyGeneratorType> like IEnergyGenerator if energyGeneratorType != "" {
            parameters:
                @display("p=50,150;i=block/plug;is=s");
        }

        srv: SrvWrapper {
            parameters:
                registerSAP = false;
                @display("p=210,68,col");
        }

        wlan: Ieee80211Nic {
           parameters:
                @display("p=120,158;q=queue");
                mgmtType = "Ieee80211MgmtSTASimplified";
        }
        mobility: StationaryMobility {
            parameters:
                @display("p=50,141");
        }
    connections allowunconnected:
        wlan.radioIn <-- radioIn;
        wlan.upperLayerOut --> srv.in;
        srv.out --> wlan.upperLayerIn;
}

network Cluster
{
    parameters:
        int numCli;
        @display("b=400,400");
    submodules:
        cliHost[numCli]: ClusterClient {
            parameters:
                @display("p=100,100;r=,,#707070");
        }
        //ap: SmartAccessPoint {
          //  parameters:
          //      @display("p=200,200;r=,,#707070");
          //      wlan[*].mgmtType = "Ieee80211MgmtAPSimplified";
        //}

        ap: AccessPoint {
            @display("p=200,200;r=,,#707070");
            wlan[*].mgmtType = "Ieee80211MgmtAPSimplified";
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=61,46");
        }

}

