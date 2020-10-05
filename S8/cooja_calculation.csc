<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/mrm</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/mspsim</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/avrora</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/serial_socket</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/collect-view</project>
  <project EXPORT="discard">[CONTIKI_DIR]/tools/cooja/apps/powertracker</project>
  <simulation>
    <title>Calculation</title>
    <speedlimit>1.0</speedlimit>
    <randomseed>123456</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      se.sics.cooja.radiomediums.UDGM
      <transmitting_range>50.0</transmitting_range>
      <interference_range>100.0</interference_range>
      <success_ratio_tx>1.0</success_ratio_tx>
      <success_ratio_rx>1.0</success_ratio_rx>
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      se.sics.cooja.mspmote.SkyMoteType
      <identifier>sky1</identifier>
      <description>Sky Mote Type #sky1</description>
      <source EXPORT="discard">[CONTIKI_DIR]/surrey/S8/calculation.c</source>
      <commands EXPORT="discard">make calculation.sky TARGET=sky</commands>
      <firmware EXPORT="copy">[CONTIKI_DIR]/surrey/S8/calculation.sky</firmware>
      <moteinterface>se.sics.cooja.interfaces.Position</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>se.sics.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>se.sics.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
    <mote>
      <breakpoints />
      <interface_config>
        se.sics.cooja.interfaces.Position
        <x>-7.515414031092613</x>
        <y>-43.82478351630446</y>
        <z>0.0</z>
      </interface_config>
      <interface_config>
        se.sics.cooja.mspmote.interfaces.MspMoteID
        <id>1</id>
      </interface_config>
      <motetype_identifier>sky1</motetype_identifier>
    </mote>
  </simulation>
  <plugin>
    se.sics.cooja.plugins.SimControl
    <width>418</width>
    <z>0</z>
    <height>160</height>
    <location_x>400</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.Visualizer
    <plugin_config>
      <skin>se.sics.cooja.plugins.skins.IDVisualizerSkin</skin>
      <viewport>0.9090909090909091 0.0 0.0 0.9090909090909091 192.83219457372056 91.84071228754951</viewport>
    </plugin_config>
    <width>400</width>
    <z>3</z>
    <height>177</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.TimeLine
    <plugin_config>
      <mote>0</mote>
      <showRadioRXTX />
      <showRadioHW />
      <showLEDs />
      <split>-1</split>
      <zoomfactor>500.0</zoomfactor>
    </plugin_config>
    <width>400</width>
    <z>4</z>
    <height>137</height>
    <location_x>0</location_x>
    <location_y>465</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.MoteInterfaceViewer
    <mote_arg>0</mote_arg>
    <plugin_config>
      <interface>Serial port</interface>
      <scrollpos>0,0</scrollpos>
    </plugin_config>
    <width>414</width>
    <z>5</z>
    <height>434</height>
    <location_x>403</location_x>
    <location_y>164</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.MoteInterfaceViewer
    <mote_arg>0</mote_arg>
    <plugin_config>
      <interface>Temperature and Light</interface>
      <scrollpos>0,0</scrollpos>
    </plugin_config>
    <width>396</width>
    <z>2</z>
    <height>163</height>
    <location_x>1</location_x>
    <location_y>182</location_y>
  </plugin>
  <plugin>
    se.sics.cooja.plugins.MoteInterfaceViewer
    <mote_arg>0</mote_arg>
    <plugin_config>
      <interface>Button</interface>
      <scrollpos>0,0</scrollpos>
    </plugin_config>
    <width>395</width>
    <z>1</z>
    <height>115</height>
    <location_x>0</location_x>
    <location_y>345</location_y>
  </plugin>
</simconf>

