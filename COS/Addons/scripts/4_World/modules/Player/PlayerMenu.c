class PlayerMenu extends PopupMenu
{
    ref array< ref PermissionRow >  m_Permissions;
    ref array< ref PlayerRow >      m_Players;

    GridSpacerWidget                m_PlayerScriptList;
    ButtonWidget                    m_ReloadScriptButton;
    GridSpacerWidget                m_PermsContainer;
    ButtonWidget                    m_SetPermissionsButton;

    void PlayerMenu()
    {
        m_Permissions = new ref array< ref PermissionRow >;
        m_Players = new ref array< ref PlayerRow >;
    }

    void ~PlayerMenu()
    {
    }

    override string GetTitle()
    {
        return "Players";
    }
    
    override string GetIconName()
    {
        return "P";
    }

    override bool ImageIsIcon()
    {
        return false;
    }

    override void Init()
    {
        m_PlayerScriptList      = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("player_list"));
        m_ReloadScriptButton    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("refresh_list"));

        m_PermsContainer        = GridSpacerWidget.Cast(layoutRoot.FindAnyWidget("PermsListCont"));
        m_SetPermissionsButton  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("permissions_set_button"));
    }

    override void OnShow()
    {
        ReloadPlayers();
    }

    override void OnHide() 
    {
    }

    override bool OnClick( Widget w, int x, int y, int button )
    {        
        if ( w == m_ReloadScriptButton )
        {
            ReloadPlayers();
        }

        if ( w == m_SetPermissionsButton )
        {
            SetPermissions();
        }

        return false;
    }

    void OnPlayerSelected( PlayerRow row )
    {
        for ( int j = 0; j < m_Players.Count(); j++ )
        {
            m_Players[j].GetLayoutRoot().SetColor(0xFF000000);

            if ( m_Players[j].m_Player.GetIdentity().GetId() == GetGame().GetPlayer().GetIdentity().GetId() )
            {
                row.GetLayoutRoot().SetColor(0x9900AA00);
            }
        }
        row.GetLayoutRoot().SetColor(0x99AAAAAA);

        SELECTED_PLAYER = row.m_Player;

        layoutRoot.FindAnyWidget("PlayerPermsContainer").Enable( false );

        if ( SELECTED_PLAYER != NULL )
        {
            GetRPCManager().SendRPC( "COS_Player", "LoadPermissions", new Param, true, NULL, SELECTED_PLAYER );
        }
    }

    void ReloadPlayers()
    {
        GetRPCManager().SendRPC( "COS_Player", "ReloadList", new Param, true );
    }

    void LoadPermissions( ref array< string > perms )
    {
        for ( int j = 0; j < m_Permissions.Count(); j++ )
        {
            m_Permissions[j].GetLayoutRoot().Unlink();
        }

        m_Permissions.Clear();

        for ( int i = 0; i < perms.Count(); i++ )
        {
            Widget permRow = GetGame().GetWorkspace().CreateWidgets( "COS/gui/layouts/player/PermissionRow.layout", m_PermsContainer );

            PermissionRow rowScript;
            permRow.GetScript( rowScript );

            if ( rowScript )
            {
                array<string> spaces = new array<string>;
                perms[i].Split( " ", spaces );

                if ( spaces.Count() != 2 ) continue;

                bool value = false;

                if ( spaces[1].ToInt() == 1 )
                {
                    value = true;
                }

                rowScript.SetPermission( spaces[0], value );

                m_Permissions.Insert( rowScript );
            }
        }
    }

    void SetPermissions()
    {
        // GetRPCManager().SendRPC( "COS_Player", "SetPermissions", new Param1< ref array< string > >( m_Permissions ), true, NULL, SELECTED_PLAYER );

        layoutRoot.FindAnyWidget("PlayerPermsContainer").Enable( true );
    }

    void UpdateList( ref array< Man > players )
    {
        for ( int j = 0; j < m_Players.Count(); j++ )
        {
            m_Players[j].GetLayoutRoot().Unlink();
        }

        m_Players.Clear();

        for ( int i = 0; i < players.Count(); i++ )
        {
            Man player = players.Get( i );

            PlayerIdentity identity = player.GetIdentity();

            Widget permRow = GetGame().GetWorkspace().CreateWidgets( "COS/gui/layouts/player/PlayerRow.layout", m_PlayerScriptList );

            PlayerRow rowScript;
            permRow.GetScript( rowScript );

            if ( rowScript )
            {
                rowScript.SetPlayer( player );

                rowScript.playerMenu = this;

                m_Players.Insert( rowScript );
            }
        }

        if ( m_Players.Count() > 0 )
        {
            OnPlayerSelected( m_Players[0] );
        }
    }
}