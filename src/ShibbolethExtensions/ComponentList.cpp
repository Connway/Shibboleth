#include "ComponentList.h"
#include <Contrivance_IContrivanceWindow.h>
#include <Contrivance_ExtensionSpawner.h>

CONTRIVANCE_EXTENSION_IMPLEMENATION(ComponentList)

ComponentList::ComponentList(IContrivanceWindow& window):
	CONTRIVANCE_EXTENSION_INITIALIZER_LIST
{
	addItem("Test Item");
}

ComponentList::~ComponentList(void)
{
	CONTRIVANCE_EXTENSION_DESTRUCTOR;
}

/*bool ComponentList::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::DragEnter) {
		_window.printToConsole("DRAG ENTER EVENT");
	} else if (event->type() == QEvent::DragResponse) {
		_window.printToConsole("DRAG RESPONSE EVENT");
	} else if (event->type() == QEvent::Drop) {
		_window.printToConsole("DROP EVENT");
	} else if (event->type() == QEvent::MouseButtonDblClick) {
		_window.printToConsole("DOUBLE CLICK");
	}

	return QObject::eventFilter(object, event);
}*/

/*
void setupUi(QWidget *ComponentList)
{
	if (ComponentList->objectName().isEmpty())
		ComponentList->setObjectName(QStringLiteral("ComponentList"));
	ComponentList->resize(400, 300);
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(ComponentList->sizePolicy().hasHeightForWidth());
	ComponentList->setSizePolicy(sizePolicy);
	horizontalLayout = new QHBoxLayout(ComponentList);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	horizontalLayout->setContentsMargins(0, 0, 0, 0);
	listWidget = new QListWidget(ComponentList);
	listWidget->setObjectName(QStringLiteral("listWidget"));
	listWidget->setFrameShape(QFrame::StyledPanel);
	listWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	listWidget->setProperty("showDropIndicator", QVariant(false));
	listWidget->setDragEnabled(true);
	listWidget->setDragDropMode(QAbstractItemView::DragOnly);
	listWidget->setDefaultDropAction(Qt::IgnoreAction);
	listWidget->setResizeMode(QListView::Adjust);
	listWidget->setSortingEnabled(true);

	horizontalLayout->addWidget(listWidget);


	retranslateUi(ComponentList);

	QMetaObject::connectSlotsByName(ComponentList);
} // setupUi
*/
