#!/usr/bin/env python

from OsogoWindow import *
from PluginInstanceSelection import *

import gtk
from ecell.ecssupport import *
import gobject

import operator
import string
import copy

# This parameter should be set by setting file.
DEFAULT_WINDOW = 'TracerWindow'


class EntityListWindow(OsogoWindow):
	"""EntityListWindow
	"""

	def __init__( self, aSession ):
		"""Constructor
		aSession   --   a reference to GtkSessionMonitor
		"""

		# calls superclass's constructor 
		OsogoWindow.__init__( self, aSession, 'EntityListWindow.glade' )

		# initializes parameters
		self.theSelectedFullPNList = []
		self.thePluginManager = aSession.thePluginManager
		self.thePluginInstanceSelection = None

	def openWindow( self ):

		# calls superclass's openWindow
		OsogoWindow.openWindow( self )


		# add handers
		self.addHandlers( { 
		                    # system tree
		                    'on_system_tree_cursor_changed' : self.updateSystemSelection,\
		                    'on_system_tree_button_press_event' : self.popupMenu,\
		                    # entity list
		                    'on_entity_tree_cursor_changed' : self.selectEntity,\
		                    'on_entity_tree_button_press_event' : self.popupMenu,\
		                    # entity menu
		                    'on_entity_optionmenu_changed' : self.updateSystemSelection,\
		                    # buttons
		                    'on_create_button_clicked'     : self.createPluginWindow,\
		                    'on_append_button_clicked'     : self.__openPluginInstanceSelectionWindow,\
		                    'on_logger_button_clicked'     : self.createLogger,\
				    'on_add_to_board_clicked'	   : self.addToBoard,\
		                    # plugin selection
		                    #'on_ok_button_plugin_selection_clicked'     : self.appendData,\
		                    #'on_cancel_button_plugin_selection_clicked'  : self.__closePluginInstanceSelectionWindow,\
		                   } )

		# calls initializes methods
		self.__initializeSystemTree()
		self.__initializeEntityTree()
		self.__initializePluginWindowOptionMenu()
		self.__initializePropertyWindow()
		self.__initializePopupMenu()

		# --------------------------------------------
		# initializes PluginInstanceSelection
		# --------------------------------------------
		#self.__initializePluginInstanceSelectonWindow()
		# at first this window should be hidden
		#self['PluginInstanceSelection'].hide()
		# sets 'delete_event' as 'hide_event'
		#self['PluginInstanceSelection'].connect("delete_event",self.deletePluginInstanceSelection)

		# --------------------------------------------
		# initializes system tree
		# --------------------------------------------
		# set up system tree
		self.theSysTreeStore.clear()

		# create route ID
		aRootSystemFullID = createFullID( 'System::/' )
		self.constructTree( None, aRootSystemFullID )
		self.update()

		# --------------------------------------------
		# initializes buffer
		# --------------------------------------------
		self.theSelectedEntityList = []
		self.theSelectedPluginInstanceList = []
		
		# --------------------------------------------
		# initializes Add to Board button
		# --------------------------------------------
		self.checkBoardExists()
		self.CloseOrder = False


	def checkBoardExists( self ):
		if self.theSession.getWindow('BoardWindow').exists():
			self['add_to_board'].set_sensitive(TRUE)
		else:
			self['add_to_board'].set_sensitive(FALSE)
		

	def deleted( self, *arg ):
		self.close()


	def close( self ):
		if self.CloseOrder:
			return
		self.CloseOrder = True

		if self.thePluginInstanceSelection != None:
			self.thePluginInstanceSelection.deleted()
			self.thePluginInstanceSelection = None
		self.theSession.deleteEntityListWindow( self )
		OsogoWindow.close(self)
	
	# ====================================================================
	def deletePluginInstanceSelection( self, *arg ):
		"""sets 'delete_event' as 'hide_event'
		"""

		# hides this window
		self['PluginInstanceSelection'].hide_all()

		# sets 'delete_event' uneffective
		return TRUE

	# ====================================================================
	def __initializeSystemTree( self ):
		"""initializes SystemTree
		"""

		self.theSysTreeStore=gtk.TreeStore( gobject.TYPE_STRING )
		self['system_tree'].set_model(self.theSysTreeStore)
		column=gtk.TreeViewColumn( 'System Tree',
					   gtk.CellRendererText(),
					   text=0 )
		column.set_visible( gtk.TRUE )
		self['system_tree'].append_column(column)


	# ====================================================================
	def __initializeEntityTree( self ):
		"""initializes EntityTree
		"""

		column = gtk.TreeViewColumn( 'Entity List',
					     gtk.CellRendererText(),
					     text=0 )
		self['entity_tree'].append_column(column)
		self.theEntityListStore=gtk.ListStore( gobject.TYPE_STRING )
		self['entity_tree'].get_selection().set_mode( gtk.SELECTION_MULTIPLE )
		self['entity_tree'].set_model( self.theEntityListStore )

	# ====================================================================
	def __initializePluginWindowOptionMenu( self ):
		"""initializes PluginWindowOptionMenu
		"""

		aPluginWindowNameList = []
		aMenu = gtk.Menu()

		for aPluginWindowName in self.thePluginManager.thePluginMap.keys():

			aButton = gtk.Button()
			aMenuItem = gtk.MenuItem(aPluginWindowName)

			if aPluginWindowName == DEFAULT_WINDOW:
				aMenu.prepend( aMenuItem )
			else:
				aMenu.append( aMenuItem )

		self['plugin_optionmenu'].set_menu(aMenu)
		self['plugin_optionmenu'].show_all()

		# set default menu

	# ====================================================================
	def __initializePropertyWindow( self ):

		self.thePropertyWindow = self.thePluginManager.createInstance( \
		         'PropertyWindow', [(SYSTEM, '', '/', '')],  self ) 
		self.thePropertyWindow.setStatusBar( self['statusbar'] )

		aPropertyWindowTopVBox = self.thePropertyWindow['top_frame']
		self['property_area'].add( aPropertyWindowTopVBox )
		self.thePropertyWindow.setParent( self )


	# ====================================================================
	def __initializePopupMenu( self ):
		"""Initialize popup menu
		Returns None
		[Note]:In this method, only 'PluginWindow type' menus, 'Create 
		       Logger' menu and 'Add to Board' menu are created. 
		       The menus of PluginWindow instances are appended
		       dinamically in self.popupMenu() method.
		"""

		# ------------------------------------------
		# menus for PluginWindow
		# ------------------------------------------

		# creaets menus of PluginWindow
		for aPluginWindowType in self.thePluginManager.thePluginMap.keys(): 
			aMenuItem = gtk.MenuItem( aPluginWindowType )
			aMenuItem.connect('activate', self.createPluginWindow )
			aMenuItem.set_name( aPluginWindowType )
			if aPluginWindowType == DEFAULT_WINDOW:
				self['EntityPopupMenu'].prepend( aMenuItem )
			else:
				self['EntityPopupMenu'].append( aMenuItem )

		# appends separator
		self['EntityPopupMenu'].append( gtk.MenuItem() )

		# ------------------------------------------
		# menus for Logger
		# ------------------------------------------
		# creates menu of Logger
		aLogMenuString = "Create Logger"
		aMenuItem = gtk.MenuItem( aLogMenuString )
		aMenuItem.connect('activate', self.createLogger )
		aMenuItem.set_name( aLogMenuString )
		self['EntityPopupMenu'].append( aMenuItem )

		# appends separator
		self['EntityPopupMenu'].append( gtk.MenuItem() )

		# ------------------------------------------
		# menus for Bord
		# ------------------------------------------
		# creates menu of Board
		aSubMenu = gtk.Menu()

		for aPluginWindowType in self.thePluginManager.thePluginMap.keys(): 
			aMenuItem = gtk.MenuItem( aPluginWindowType )
			aMenuItem.connect('activate', self.addToBoard )
			aMenuItem.set_name( aPluginWindowType )
			if aPluginWindowType == DEFAULT_WINDOW:
				aSubMenu.prepend( aMenuItem )
			else:
				aSubMenu.append( aMenuItem )

		aMenuString = "Add to Board"
		aMenuItem = gtk.MenuItem( aMenuString )
		aMenuItem.set_name( aLogMenuString )
		aMenuItem.set_submenu( aSubMenu )
		self['EntityPopupMenu'].append( aMenuItem )
		self.theBoardMenu = aMenuItem

		# appends separator
		self['EntityPopupMenu'].append( gtk.MenuItem() )

		# ------------------------------------------
		# menus for submenu
		# ------------------------------------------
		self.thePopupSubMenu = None  


	# ====================================================================
	#def __initializePluginInstanceSelectonWindow( self ):
	#	"""initializes PluginInstanceSelectionWindow
	#	Returns None
	#	"""

	#	column = gtk.TreeViewColumn( 'Plugin List',
	#				     gtk.CellRendererText(),
	#				     text=0 )
	#	self['plugin_tree'].append_column(column)
	#	self.thePluginInstanceListStore=gtk.ListStore( gobject.TYPE_STRING )
	#	self['plugin_tree'].get_selection().set_mode( gtk.SELECTION_MULTIPLE )
	#	self['plugin_tree'].set_model( self.thePluginInstanceListStore )
	#	column = gtk.TreeViewColumn( 'Plugin List',
	#				     gtk.CellRendererText(),
	#				     text=0 )

	# ====================================================================
	def __openPluginInstanceSelectionWindow( self, *arg ):
		"""open PluginInstanceSelectionWindow
		Returns None
		"""

		if self.thePluginInstanceSelection != None:
			self.thePluginInstanceSelection.present()

		else:

			self.thePluginInstanceSelection = \
			PluginInstanceSelection( self.theSession, self )
			self.thePluginInstanceSelection.openWindow()

			# updates list of PluginInstance
			self.thePluginInstanceSelection.update()

			# displays it
			#self['PluginInstanceSelection'].show_all()
			# moves it to the top of desktop
			#self['PluginInstanceSelection'].present()


	# ====================================================================
	def __updatePluginInstanceSelectionWindow2( self ):
		"""updates list of PluginInstanceSelectionWindow
		Returns None
		"""

		self.thePluginInstanceListStore.clear()
		aPluginInstanceList = self.thePluginManager.thePluginTitleDict.keys()

		for aPluginInstance in aPluginInstanceList:
			if aPluginInstance.theViewType == MULTIPLE:
				aPluginInstanceTitle = self.thePluginManager.thePluginTitleDict[aPluginInstance]
				iter = self.thePluginInstanceListStore.append()
				self.thePluginInstanceListStore.set_value( iter, 0, aPluginInstanceTitle )
				self.thePluginInstanceListStore.set_data( aPluginInstanceTitle, aPluginInstanceTitle )

	# ====================================================================
	def closePluginInstanceSelectionWindow( self, *arg ):
		"""closes PluginInstanceSelectionWindow
		Returns None
		"""

		if self.thePluginInstanceSelection != None:
			#self.thePluginInstanceSelection['PluginInstanceSelection'].hide_all()
			self.thePluginInstanceSelection.deleted()
			self.thePluginInstanceSelection = None


	# ====================================================================
	def popupMenu( self, aWidget, anEvent ):
		"""displays popup menu only when right button is pressed.
		aWidget   --  EntityListWindow
		anEvent   --  an event
		Returns None
		[Note]:creates and adds submenu that includes menus of PluginWindow instances
		"""

		# When right button is pressed
		if anEvent.type == gtk.gdk.BUTTON_PRESS and anEvent.button == 3:

			if self.theSession.getWindow('BoardWindow').exists():
				self.theBoardMenu.set_sensitive(TRUE)
			else:
				self.theBoardMenu.set_sensitive(FALSE)

			# removes previous sub menu
			# When PopupMenu was displayed last time without PluginWindows'
			# menus, the buffer (self.thePopupSubMenu) is None.
			if self.thePopupSubMenu != None:
				self['EntityPopupMenu'].remove( self.thePopupSubMenu )

			if len(self.thePluginManager.theInstanceList)!=0:

				# creates submenu
				aSubMenu = gtk.Menu()

				# creaets menus of PluginWindow instances
				aMenuItemFlag = FALSE
				for aPluginInstance in self.thePluginManager.theInstanceList: 
		
					if aPluginInstance.theViewType == MULTIPLE:
						aTitle = aPluginInstance.getTitle()
						aMenuItem = gtk.MenuItem( aTitle )
						aMenuItem.connect('activate', self.appendData )
						aMenuItem.set_name( aTitle )
						aSubMenu.append( aMenuItem )
						aMenuItemFlag = TRUE

				if aMenuItemFlag == TRUE:
					# creates parent MenuItem attached created submenu.
					aMenuString = "Append data to"
					aMenuItem = gtk.MenuItem( aMenuString )
					aMenuItem.set_submenu( aSubMenu )

					# appends parent MenuItem to PopupMenu
					self['EntityPopupMenu'].append( aMenuItem )

					# saves this submenu set to buffer (self.thePopupSubMenu)
					self.thePopupSubMenu = aMenuItem


			# displays all items on PopupMenu
			self['EntityPopupMenu'].show_all() 

			# displays popup menu
			self['EntityPopupMenu'].popup(None,None,None,anEvent.button,anEvent.time)


	# ====================================================================
	def update( self ):
		"""overwrite superclass's method
		updates this window and property window
		Returns None
		"""

		# updates this window
		OsogoWindow.update(self)

		# updates property window
		self.thePropertyWindow.update()

		# update PluginInstanceSelectionWindow
		if self.thePluginInstanceSelection != None:
			self.thePluginInstanceSelection.update()


	# ========================================================================
	def constructTree( self, aParentTree, aSystemFullID ):


		aNewlabel = aSystemFullID[ID] 

		iter  = self.theSysTreeStore.append( aParentTree )
		self.theSysTreeStore.set_value( iter, 0, aNewlabel )
		key = str( self.theSysTreeStore.get_path( iter ) )
		self.theSysTreeStore.set_data( key, aSystemFullID )
		    
		aSystemPath = createSystemPathFromFullID( aSystemFullID )
		aSystemList = self.theSession.getEntityList( 'System',\
							     aSystemPath )
		aSystemListLength = len( aSystemList )

		if  aSystemListLength == 0:
			return
		
		for aSystemID in aSystemList:
			aNewSystemFullID = ( SYSTEM, aSystemPath, aSystemID )
			self.constructTree( iter, aNewSystemFullID )

			if aSystemListLength <= 5:
				aPath = self.theSysTreeStore.get_path( iter )
				self['system_tree'].expand_row( aPath, gtk.FALSE )

	# ========================================================================
	def updateSystemSelection( self, obj=None ):
		aSelectedSystemIter = self['system_tree'].get_selection().get_selected()[1]
		if aSelectedSystemIter == None:
			return None

		self.updateEntityList( aSelectedSystemIter )

		if type(obj) == gtk.TreeView:
			anEntityTypeString = self['entity_optionmenu'].get_children()[0].get()
			key=str(self.theSysTreeStore.get_path(aSelectedSystemIter))
			aSystemFullID = self.theSysTreeStore.get_data( key )
			aSystemPath = createSystemPathFromFullID( aSystemFullID )
			aEntityList = self.theSession.getEntityList( 'System', aSystemPath )
			self['label2'].set_text('System ('+str(len(aEntityList))+')')

			aFullPN =  convertFullIDToFullPN(aSystemFullID) 
			self.thePropertyWindow.setRawFullPNList( [convertFullIDToFullPN(aSystemFullID)] )
		self.checkCreateLoggerButton()

	# ========================================================================
	def updateEntityList( self, aSelectedSystemIter ):

		anEntityTypeString = self['entity_optionmenu'].get_children()[0].get()
		key=str(self.theSysTreeStore.get_path(aSelectedSystemIter))
		aSystemFullID = self.theSysTreeStore.get_data( key )
		self.theEntityListStore.clear()

		if anEntityTypeString == 'All Entities':
			self.listEntity( 'Variable', aSystemFullID )
			self.listEntity( 'Process', aSystemFullID )
		else:
			self.listEntity( anEntityTypeString, aSystemFullID )


	# ========================================================================
	# create list of Entity tree
	def listEntity( self, aEntityTypeString, aSystemFullID ):
		aSystemPath = createSystemPathFromFullID( aSystemFullID )
		aEntityList = self.theSession.getEntityList( aEntityTypeString, aSystemPath )
		if aEntityTypeString == 'Variable':
			aEntityType = VARIABLE
		elif aEntityTypeString == 'Process':
			aEntityType = PROCESS
		for anEntityID in aEntityList:
			iter = self.theEntityListStore.append()
			self.theEntityListStore.set_value(iter,0,anEntityID)
			aEntityFullPN = ( aEntityType, aSystemPath, anEntityID, '' )
			self.theEntityListStore.set_data( anEntityID, aEntityFullPN )
		self['label3'].set_text('Entity (' + str(len(aEntityList)) + ')' )


	# ========================================================================
	def selectEntity( self, aEntityList ):
		self.theSelectedFullPNList = []
		aSelection = aEntityList.get_selection()
		aSelection.selected_foreach(self.entity_select_func)
		if len(self.theSelectedFullPNList)>0:
			self.thePropertyWindow.setRawFullPNList( [self.theSelectedFullPNList[0]] )
		self.checkCreateLoggerButton()

	# ========================================================================
	def	checkCreateLoggerButton(self):
		isSensitive = gtk.FALSE
		loggerList = self.theSession.getLoggerList()
		rawList = self.__getSelectedRawFullPNList()
		if rawList != None:
			for aFullPN in rawList:
				if aFullPN[3] == '':
					aFullPN = self.thePropertyWindow.supplementFullPN( aFullPN )
				aFullPNString = createFullPNString( aFullPN )
				aValue = self.theSession.theSimulator.getEntityProperty( aFullPNString )
				if not operator.isNumberType( aValue ):
					continue
				if aFullPNString not in loggerList:
					isSensitive = gtk.TRUE
					break
		self['logger_button'].set_sensitive( isSensitive )

	# ========================================================================
	def entity_select_func(self,tree,path,iter):
		key=self.theEntityListStore.get_value(iter,0)
		aEntityFullPN = self.theEntityListStore.get_data( key )
		self.theSelectedFullPNList.append( aEntityFullPN )

	# end of entity_select_func

	# ========================================================================
	#def plugin_select_func(self,tree,path,iter):
	#	key=self.thePluginInstanceListStore.get_value(iter,0)
	#	aTitle = self.thePluginInstanceListStore.get_data( key )
	#	self.theSelectedPluginInstanceList.append( aTitle )


	# ========================================================================
	def createPluginWindow( self, *obj ) :
		"""creates new PluginWindow instance(s)
		*obj   --  gtk.MenuItem on popupMenu or gtk.Button
		Returns None
		"""

		self.thePropertyWindow.clearStatusBar()

		if len(obj) == 0:
			return None

		aPluginWindowType = DEFAULT_WINDOW
		aSetFlag = FALSE

		# When this method is called by popup menu
		if type( obj[0] ) == gtk.MenuItem:
			aPluginWindowType = obj[0].get_name()

		# When this method is called by 'CreateWindow' button
		elif type( obj[0] ) == gtk.Button:
			aPluginWindowType = self['plugin_optionmenu'].get_children()[0].get()

		else:
			raise TypeErrir("%s must be gtk.MenuItem or gtk.Button" %str(type(obj[0])))

		aSelectedRawFullPNList = self.__getSelectedRawFullPNList()

		# When no FullPN is selected, displays error message.
		if aSelectedRawFullPNList  == None or len( aSelectedRawFullPNList ) == 0:

			aMessage = 'No entity is selected.'
			aDialog = ConfirmWindow(OK_MODE,aMessage,'Error!')
			self.thePropertyWindow.showMessageOnStatusBar(aMessage)
			return FALSE

		self.thePropertyWindow.setRawFullPNList( aSelectedRawFullPNList )
		self.thePluginManager.createInstance( aPluginWindowType, self.thePropertyWindow.theFullPNList() )


	# ========================================================================
	def selectPropertyName( self, aCList, row, column, event_obj ):

		self.theSelectedFullPNList = []
		for aRowNumber in aCList.selection:
			aPropertyName =  aCList.get_text( aRowNumber, 0 )
			aFullID = self.thePropertyWindow.theFullID()
			aFullPN = convertFullIDToFullPN( aFullID, aPropertyName )
			self.theSelectedFullPNList.append( aFullPN )
			self.updateStatusBar()

	# end of selectPropertyName


	# ========================================================================
	def appendData( self, *obj ):
		"""appends RawFullPN to PluginWindow instance
		Returns TRUE(when appened) / FALSE(when not appened)
		"""

		# clear status bar
		self.thePropertyWindow.clearStatusBar()

		if len(obj) == 0:
			return None

		# Only when at least one menu is selected.

		# ----------------------------------------------------
		# When this method is called by popup menu
		# ----------------------------------------------------
		if type( obj[0] ) == gtk.MenuItem:
			aSetFlag = TRUE
			aPluginWindowTitle = obj[0].get_name()

			for anInstance in self.thePluginManager.theInstanceList:
				if anInstance.getTitle() == aPluginWindowTitle:

					try:
						anInstance.appendRawFullPNList( self.__getSelectedRawFullPNList() )
					except TypeError:
						anErrorFlag = TRUE
						aMessage = "Can't append data to %s" %str(anInstance.getTitle())
						self.thePropertyWindow.showMessageOnStatusBar(aMessage)
					else:
						aMessage = "Selected Data are added to %s" %aPluginWindowTitle
						self.thePropertyWindow.showMessageOnStatusBar(aMessage)
					break
					
			return TRUE

		# ----------------------------------------------------
		# When this method is called by PluginInstanceWindow
		# ----------------------------------------------------
		elif type( obj[0] ) == gtk.Button:

			self.theSelectedPluginInstanceList = []
			selection=self.thePluginInstanceSelection['plugin_tree'].get_selection()
			selection.selected_foreach(self.thePluginInstanceSelection.plugin_select_func)

			# When no FullPN is selected, displays error message.
			if self.__getSelectedRawFullPNList() == None or len( self.__getSelectedRawFullPNList() ) == 0:

				aMessage = 'No entity is selected.'
				aDialog = ConfirmWindow(OK_MODE,aMessage,'Error!')
				self.thePropertyWindow.showMessageOnStatusBar(aMessage)
				return FALSE

			# When no plugin instance is selected, displays error message.
			if len(self.theSelectedPluginInstanceList) == 0:

				aMessage = 'No Plugin Instance is selected.'
				aDialog = ConfirmWindow(OK_MODE,aMessage,'Error!')
				self.thePropertyWindow.showMessageOnStatusBar(aMessage)
				return FALSE

			# buffer of appended instance's title
			anAppendedTitle = []

			anErrorFlag = FALSE

			# appneds data
			for aPluginWindowTitle in self.theSelectedPluginInstanceList:
				for anInstance in self.thePluginManager.theInstanceList:
					if anInstance.getTitle() == aPluginWindowTitle:
						try:
							anInstance.appendRawFullPNList( self.__getSelectedRawFullPNList() )
						except TypeError:
							anErrorFlag = TRUE
							aMessage = "Can't append data to %s" %str(anInstance.getTitle())
							self.thePropertyWindow.showMessageOnStatusBar(aMessage)
						else:
							anAppendedTitle.append( anInstance.getTitle() )
						break

			# When at least one instance is appended
			if len(anAppendedTitle) > 0 and anErrorFlag == FALSE:
				# displays message
				aMessage = "Selected Data are added to %s" %str(anAppendedTitle)
				self.theSession.message(aMessage)
				self.thePropertyWindow.showMessageOnStatusBar(aMessage)

				# closes PluginInstanceSelectionWindow
				#self.__closePluginInstanceSelectionWindow()
				self.closePluginInstanceSelectionWindow()
				return TRUE

			# When no instance is appended
			else:

				return NONE


	# ========================================================================
	def __getSelectedRawFullPNList( self ):
		"""returns selected FullPNList
		If no property is selected on PropertyWindow, return FullPN
		Returns FullPNList
		"""

		self.theSelectedFullPNList = []

		selection=self['entity_tree'].get_selection()
		selection.selected_foreach(self.entity_select_func)

		if len(self.theSelectedFullPNList) == 0:
			aSelectedSystemIter = self['system_tree'].get_selection().get_selected()[1]
			if aSelectedSystemIter != None:
				key=str(self.theSysTreeStore.get_path(aSelectedSystemIter))
				aSystemFullID = self.theSysTreeStore.get_data( key )
				self.theSelectedFullPNList = [(aSystemFullID[0],aSystemFullID[1],aSystemFullID[2],'')]


		# -------------------------------------------------------------------
		# If no property is selected on PropertyWindow, create plugin Window
		# with default property (aValue) 
		# -------------------------------------------------------------------
		if len( str(self.thePropertyWindow.getSelectedFullPN()) ) == 0:  # if(1)
			return self.theSelectedFullPNList

		# ----------------------------------------------------------------
		# If a property is selected on PropertyWindow, create plugin Window
		# with selected property
		# ----------------------------------------------------------------
		else:  # if(1)
			return [self.thePropertyWindow.getSelectedFullPN()]

#			aSpecifiedProperty = self.thePropertyWindow.getSelectedFullPN()[PROPERTY]
#			# buffer list for FullPN that doen not have specified property aNoPropertyFullIDList = []
#			aSelectedFullPNListWithSpecified = []
#			aNoPropertyFullIDList = []
##			for aSelectedFullPN in self.theSelectedFullPNList:
#				aFullID = convertFullPNToFullID(aSelectedFullPN) 
#				aFullIDString = createFullIDString( aFullID )
#				anEntityStub = EntityStub(self.theSession.theSimulator,aFullIDString)
#				aPropertyExistsFlag = FALSE
#				for aProperty in anEntityStub.getPropertyList():
#					if aProperty == aSpecifiedProperty:
#						aFullPN = convertFullIDToFullPN( aFullID, aSpecifiedProperty )
#						aSelectedFullPNListWithSpecified.append( aFullPN )
#						aPropertyExistsFlag = TRUE
#						break
#				if aPropertyExistsFlag == FALSE:
#					aNoPropertyFullIDList.append( aFullIDString )
#
#			# When some selected Entity does not have specified property,
#			# shows confirmWindow and does not create plugin window.
#			if len(aNoPropertyFullIDList) != 0:
#
#				# creates message
#				aMessage = ''
#				# one entity
#				if len(aNoPropertyFullIDList) == 1:
#					aMessage += ' The following Entity does not have %s \n' %aSpecifiedProperty
#				# entities
#				else:
#					aMessage += ' The following Entities do not have %s \n' %aSpecifiedProperty
#
#				for aNoPropertyFullIDString in aNoPropertyFullIDList:
#					aMessage += aNoPropertyFullIDString + '\n'
#
#				# print message to message 
#				aDialog = ConfirmWindow(OK_MODE,aMessage,'Error!')
#				return None
#
#			# creates plugin window
#			#self.thePluginManager.createInstance( aPluginWindowType, aSelectedFullPNListWithSpecified )
#			return aSelectedFullPNListWithSpecified



	# ========================================================================
	def addToBoard( self, *arg ):
		"""add plugin window to board
		"""

		self.thePropertyWindow.clearStatusBar()

		if len(arg) == 0:
			return None

		aPluginWindowType = DEFAULT_WINDOW
		aSetFlag = FALSE

		# When this method is called by popup menu
		if type( arg[0] ) == gtk.MenuItem:
			aPluginWindowType = arg[0].get_name()

		# When this method is called by 'CreateWindow' button
		elif type( arg[0] ) == gtk.Button:
			aPluginWindowType = self['plugin_optionmenu'].get_children()[0].get()

		else:
			raise TypeError("%s must be gtk.MenuItem or gtk.Button" %str(type(arg[0])))

		aSelectedRawFullPNList = self.__getSelectedRawFullPNList()

		# When no FullPN is selected, displays error message.
		if aSelectedRawFullPNList  == None or len( aSelectedRawFullPNList ) == 0:

			aMessage = 'No entity is selected.'
			aDialog = ConfirmWindow(OK_MODE,aMessage,'Error!')
			self.thePropertyWindow.showMessageOnStatusBar(aMessage)
			return FALSE

		self.theSession.getWindow('BoardWindow').addPluginWindows( aPluginWindowType, \
		self.__getSelectedRawFullPNList() )


	# ========================================================================
	def createLogger( self, *arg ):
		"""creates Logger about all FullPN selected on EntityTreeView
		Returns None
		"""

		# clear status bar
		self.thePropertyWindow.clearStatusBar()

		# gets selected RawFullPNList
		aSelectedRawFullPNList = self.__getSelectedRawFullPNList()


		# When no entity is selected, displays confirm window.
		if len(aSelectedRawFullPNList) == 0:

			# print message to message 
			aMessage = 'No Entity is selected.'
			self.thePropertyWindow.showMessageOnStatusBar(aMessage)
			aDialog = ConfirmWindow(OK_MODE,aMessage,'Error!')
			return None

		# creates Logger using PropertyWindow
		self.thePropertyWindow.setRawFullPNList( aSelectedRawFullPNList )
		self.thePropertyWindow.createLogger()

		# display message on status bar
		if len(aSelectedRawFullPNList) == 1:
			aMessage = 'Logger was created.'
		else:
			aMessage = 'Loggers were created.'
		self.thePropertyWindow.showMessageOnStatusBar(aMessage)
		self.checkCreateLoggerButton()


	# ========================================================================
	def selectPropertyName( self ):
		"""selects property name
		Returns None
		"""
		self.checkCreateLoggerButton()
	# end of createLogger

