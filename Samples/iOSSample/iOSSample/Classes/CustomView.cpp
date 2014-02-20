//
//  CustomView.cpp
//  MoneyMobileX
//
//  Created by Saul Howard on 12/2/13.
//
//

#include "CustomView.h"

void CustomView::onDraw(shared_ptr<Canvas> canvas) {
    View::onDraw(canvas);
    
    canvas->drawText(m_text.getBuffer(), 0, m_text.length(), 0.0f, getHeight() / 2.0f, 0, &m_paint);
    canvas->drawRect(m_width, 0, getWidth(), getHeight(), &m_paint);
}