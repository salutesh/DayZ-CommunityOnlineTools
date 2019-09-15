enum JMMapModuleRPC
{
    INVALID = 10180,
    Teleport
    COUNT
};

class JMMapModule: JMRenderableModuleBase
{
	protected ref array< Man > m_ServerPlayers;
	
	void JMMapModule() 
	{
		m_ServerPlayers = new array< Man >;

		GetPermissionsManager().RegisterPermission( "Map.View" );
		GetPermissionsManager().RegisterPermission( "Map.Teleport" );
		GetPermissionsManager().RegisterPermission( "Map.Show.Players" );
	}
	
	override string GetLayoutRoot()
	{
		return "JM/COT/GUI/layouts/mapview_form.layout";
	}

	override bool HasAccess()
	{
		return GetPermissionsManager().HasPermission( "Map.View" );
	}

	int GetRPCMin()
	{
		return JMMapModuleRPC.INVALID;
	}

	int GetRPCMax()
	{
		return JMMapModuleRPC.COUNT;
	}

	override void OnRPC( PlayerIdentity sender, Object target, int rpc_type, ref ParamsReadContext ctx )
	{
		switch ( rpc_type )
		{
		case JMMapModuleRPC.Teleport:
			RPC_Teleport( ctx, sender, target );
			break;
		}
    }

	void Teleport( vector position )
	{
		if ( IsMissionClient() )
		{
			Client_Teleport( position );
		}
	}

	private void Client_Teleport( vector position )
	{
		if ( !GetPermissionsManager().HasPermission( "Map.Teleport" ) )
			return;

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write( new Param1< vector >( position ) );
		rpc.Send( NULL, JMMapModuleRPC.Teleport, true, NULL );
	}

	private void Server_Teleport( vector position, PlayerBase player )
	{
		if ( !GetPermissionsManager().HasPermission( "Map.Teleport", player.GetIdentity() ) )
			return;

		player.SetLastPosition( player.GetPosition() );

		if ( player.IsInTransport() )
		{
			HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();

			if ( vehCommand )
			{
				Transport transport = vehCommand.GetTransport();

				if ( transport == NULL )
					return;

				transport.SetPosition( position );
			}
		} else
		{
			player.SetPosition( position );
		}

		GetCommunityOnlineToolsBase().Log( player.GetIdentity(), "Teleported to position " + position + " from map." );
	}

	private void RPC_Teleport( ref ParamsReadContext ctx, PlayerIdentity senderRPC, Object target )
	{
		if ( IsMissionHost() )
		{
			Param1< vector > pos;
			if ( !ctx.Read( pos ) )
			{
				return;
			}

			PlayerBase player = GetPlayerObjectByIdentity( senderRPC );

			if ( !player )
				return;

			Server_Teleport( pos.param1, player );
		}
	}
}