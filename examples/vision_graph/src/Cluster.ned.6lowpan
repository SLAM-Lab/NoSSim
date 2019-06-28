//Interface module/wrapper between OMNET++ MAC/PHY layers and full system simulator 
import NoS.applications.ethernet.CliWrapper;
import NoS.applications.ethernet.SrvWrapper;

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
        wlan: Ieee802154NarrowbandNic {
            parameters:
                @display("p=120,158;q=queue");
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
                @display("p=210,68,col");
        }

        wlan: Ieee802154NarrowbandNic {
           parameters:
                @display("p=120,158;q=queue");
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
        //srvHost: ClusterServer {
        //    parameters:
        //        @display("p=150,150;r=,,#707070");
        //}
        radioMedium: Ieee802154NarrowbandScalarRadioMedium {
            parameters:
               @display("p=61,46");
        }

}

