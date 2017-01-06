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
			<Item Name="Plot Polar Data.vi" Type="VI" URL="../Sub-VIs/Plot Polar Data.vi"/>
			<Item Name="Polar Plot with Point Options.vi" Type="VI" URL="../Sub-VIs/Polar Plot with Point Options.vi"/>
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
				<Item Name="Draw Legend.vi" Type="VI" URL="/&lt;vilib&gt;/picture/radarplt.llb/Draw Legend.vi"/>
				<Item Name="Draw Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Rect.vi"/>
				<Item Name="Set Pen State.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Set Pen State.vi"/>
				<Item Name="Draw Text in Rect(6_1).vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Text in Rect(6_1).vi"/>
				<Item Name="PCT Pad String.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/PCT Pad String.vi"/>
				<Item Name="Map Value to Pixel.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Map Value to Pixel.vi"/>
				<Item Name="Draw Multiple Lines.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Multiple Lines.vi"/>
				<Item Name="Draw Polar Grids.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/Draw Polar Grids.vi"/>
				<Item Name="Get Ready.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/Get Ready.vi"/>
				<Item Name="Num To Text.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Num To Text.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Map Setup.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Map Setup.vi"/>
				<Item Name="Calc Scale Specs.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Calc Scale Specs.vi"/>
				<Item Name="Calc Increment.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Calc Increment.vi"/>
				<Item Name="Increment Filter.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Increment Filter.vi"/>
				<Item Name="PG scale.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/PG scale.vi"/>
				<Item Name="Hilite Color.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Hilite Color.vi"/>
				<Item Name="Draw Scale.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Draw Scale.vi"/>
				<Item Name="Move Pen.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Move Pen.vi"/>
				<Item Name="Draw Line.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Line.vi"/>
				<Item Name="Draw Text at Point.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Text at Point.vi"/>
				<Item Name="Draw Text in Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Text in Rect.vi"/>
				<Item Name="PG circles.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/PG circles.vi"/>
				<Item Name="Draw Circle by Radius.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Draw Circle by Radius.vi"/>
				<Item Name="Draw Arc.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Draw Arc.vi"/>
				<Item Name="PG angle labels.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/PG angle labels.vi"/>
				<Item Name="PG angle lines.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/PG angle lines.vi"/>
				<Item Name="Validate Rectangle.vi" Type="VI" URL="/&lt;vilib&gt;/picture/scale.llb/Validate Rectangle.vi"/>
				<Item Name="Autoscale Polar as Needed.vi" Type="VI" URL="/&lt;vilib&gt;/picture/polarplt.llb/Autoscale Polar as Needed.vi"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
