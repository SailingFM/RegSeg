function createParamWin(mainFig)



tamX = 256;
tamY = 210;
scrsize = get( 0, 'ScreenSize' );

x0 = scrsize(3)/2 - tamX/2;
y0 = scrsize(4)/2 - tamY/2;
% Main window -------------------------------------------------------------
hWin = figure('units','pixels','Position',[x0   y0   tamX   tamY],...
         'menubar', 'none','name','Selection settings...','numbertitle','off',...
         'resize','off');
% First pop-up menu -------------------------------------------------------     
uicontrol('Parent',hWin,'Style','text','String','Pixels/class',...
    'Units','normalized','Position', [0.15 0.68 0.4 0.2],...
    'BackgroundColor', [0.8 0.8 0.8]);  

pixels = {1:100};
hList1 = uicontrol('Parent',hWin,'Style', 'popupmenu',...
         'String',pixels,'Value',8,...
         'tag','npix',...
         'units','normalized','Position',[0.55 0.7 0.4 0.2]);
% Second pop-up menu ------------------------------------------------------     
uicontrol('Parent',hWin,'Style','text','String','# Classes',...
    'Units','normalized','Position', [0.02 0.65 0.51 0.08],...
    'BackgroundColor', [0.8 0.8 0.8]);

hList2 = uicontrol('Parent',hWin,'Style', 'popupmenu',...
         'String',{'2','3','4','5','6','7'},'Value',2,...
         'tag','classeslist',...
         'units','normalized','Position',[0.55 0.55 0.4 0.2]);

     % Panel--------------------------------------------------------------------     
hPanel = uipanel('Parent',hWin,'Backgroundcolor',[0.8 0.8 0.8],...
    'Units','normalized','Position',[0.1 0.005 0.86 0.62]);
    
uicontrol('Parent',hWin,'Style','text','String','Names of classes',...
    'Units','normalized','Position', [0.125 0.2 0.38 0.2],...
    'BackgroundColor', [0.8 0.8 0.8]);

hdl.edt1 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','CSF',...
                'HorizontalAlignment','left','Position',[0.55 0.5 0.4 0.1]);
hdl.edt2 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','WM',...
                'HorizontalAlignment','left','Position',[0.55 0.38 0.4 0.1]);
hdl.edt3 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','GM',...
                'HorizontalAlignment','left','Position',[0.55 0.26 0.4 0.1]);
hdl.edt4 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','Dura',...
                'HorizontalAlignment','left','Position',[0.55 0.14 0.4 0.1],'value',1);
hdl.edt5 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','Fat',...
                'HorizontalAlignment','left','Position',[0.55 0.02 0.4 0.1],'value',1);
hdl.edt6 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','Muscle',...
                'HorizontalAlignment','left','Position',[0.55 -0.02 0.4 0.1],'value',1);
hdl.edt7 = uicontrol('Parent',hWin,'Style','edit',...
                'Units','normalized','String','Air',...
                'HorizontalAlignment','left','Position',[0.55 -0.14 0.4 0.1],'value',1);

hOk = uicontrol('Parent',hPanel,'Style','pushbutton','string','Ok',...
    'enable','on','Units','normalized','Position',[0.08 0.1 0.16 0.2],...
    'callback',sprintf('select_popupmenu(%d,%d)',gcf,mainFig));


end

