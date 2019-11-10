# drawApp 
  version 002
## intorduce:
+ SvgTool class : 
1.  use m_handle_rect to control the size  and rotation of SvgTool::m_selected_group.
1.  use m_selected_group to store and modify selected QGraphicsItem.
  
## feature
+   You can rotate ,move ,delete ,scale the QGraphicsItem objects.
## bug 
   1. About move : The SvgTool::m_handle_rect will redraw ,if you move a item. 
   