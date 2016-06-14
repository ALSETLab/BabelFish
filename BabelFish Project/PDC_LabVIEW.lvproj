<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="13008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="CTRLs" Type="Folder">
			<Item Name="PMU_CFG Class.ctl" Type="VI" URL="../CTRLs/PMU_CFG Class.ctl"/>
			<Item Name="PMU_CMDs.ctl" Type="VI" URL="../CTRLs/PMU_CMDs.ctl"/>
			<Item Name="DIG_Display.ctl" Type="VI" URL="../CTRLs/DIG_Display.ctl"/>
		</Item>
		<Item Name="Sub-VIs" Type="Folder">
			<Item Name="DataChosen.vi" Type="VI" URL="../Sub-VIs/DataChosen.vi"/>
			<Item Name="DATA_RSHP.vi" Type="VI" URL="../Sub-VIs/DATA_RSHP.vi"/>
			<Item Name="CHK_CRC.vi" Type="VI" URL="../Sub-VIs/CHK_CRC.vi"/>
			<Item Name="CMD.vi" Type="VI" URL="../Sub-VIs/CMD.vi"/>
			<Item Name="CRC.vi" Type="VI" URL="../Sub-VIs/CRC.vi"/>
			<Item Name="DATA_CONV.vi" Type="VI" URL="../Sub-VIs/DATA_CONV.vi"/>
			<Item Name="SYS_INFO.vi" Type="VI" URL="../Sub-VIs/SYS_INFO.vi"/>
			<Item Name="UNIX-DATE_TIME.vi" Type="VI" URL="../Sub-VIs/UNIX-DATE_TIME.vi"/>
			<Item Name="PMU_INFO.vi" Type="VI" URL="../Sub-VIs/PMU_INFO.vi"/>
			<Item Name="PMU_CFG-2_PACK.vi" Type="VI" URL="../Sub-VIs/PMU_CFG-2_PACK.vi"/>
			<Item Name="ARNG_FRAME.vi" Type="VI" URL="../Sub-VIs/ARNG_FRAME.vi"/>
			<Item Name="PMU_DATA_PACK.vi" Type="VI" URL="../Sub-VIs/PMU_DATA_PACK.vi"/>
			<Item Name="HEX_DEC.vi" Type="VI" URL="../Sub-VIs/HEX_DEC.vi"/>
			<Item Name="NamesofPMUsElements.vi" Type="VI" URL="../Sub-VIs/NamesofPMUsElements.vi"/>
		</Item>
		<Item Name="Screenshots" Type="Folder" URL="../Screenshots">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="License" Type="Folder" URL="../License">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="PDC.vi" Type="VI" URL="../PDC.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="ex_CorrectErrorChain.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/ex_CorrectErrorChain.vi"/>
				<Item Name="subDisplayMessage.vi" Type="VI" URL="/&lt;vilib&gt;/express/express output/DisplayMessageBlock.llb/subDisplayMessage.vi"/>
				<Item Name="System Exec.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/system.llb/System Exec.vi"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
