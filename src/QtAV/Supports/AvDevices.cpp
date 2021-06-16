#include <qtav.h>

QActionGroup * N::Acoustics::DeviceMenu (
                  Plan    * plan        ,
                  Equipment equipment   ,
                  QMenu   * menu        )
{
  NAMEs      Names                                                           ;
  ByteArrays cameraNames                                                     ;
  UUIDs      auids                                                           ;
  SUID       uuid                                                            ;
  QString    AudioDevice = ""                                                ;
  int        DeviceId    = -1                                                ;
  QAction  * aa                                                              ;
  ////////////////////////////////////////////////////////////////////////////
  menu->clear()                                                              ;
  ////////////////////////////////////////////////////////////////////////////
  switch (equipment)                                                         {
    case Input                                                               :
      Names = Devices( Input  )                                              ;
      if (Names.count()<=0)                                                  {
        aa  = menu -> addAction  ( QObject::tr("No audio input device") )    ;
        aa         -> setEnabled ( false                                )    ;
        return NULL                                                          ;
      }                                                                      ;
      if (plan->Equipments.contains(N::Plan::DefaultAudioInput))             {
        AudioDevice = plan->Equipments[N::Plan::DefaultAudioInput]           ;
      }                                                                      ;
      if (plan->EquipmentIDs.contains(N::Plan::DefaultAudioInput))           {
        DeviceId = plan->EquipmentIDs[N::Plan::DefaultAudioInput]            ;
      }                                                                      ;
    break                                                                    ;
    case Output                                                              :
      Names = Devices( Output )                                              ;
      if (Names.count()<=0)                                                  {
        aa  = menu -> addAction  ( QObject::tr("No audio output device") )   ;
        aa         -> setEnabled ( false                                 )   ;
        return NULL                                                          ;
      }                                                                      ;
      if (plan->Equipments.contains(N::Plan::DefaultAudioOutput))            {
        AudioDevice = plan->Equipments[N::Plan::DefaultAudioOutput]          ;
      }                                                                      ;
      if (plan->EquipmentIDs.contains(N::Plan::DefaultAudioOutput))          {
        DeviceId = plan->EquipmentIDs[N::Plan::DefaultAudioOutput]           ;
      }                                                                      ;
    break                                                                    ;
    case Camera                                                              :
      cameraNames = N::Manager::Cameras()                                    ;
      if (cameraNames.count()>0)                                             {
        for (int i=0;i<cameraNames.count();i++)                              {
          Names[i] = QString::fromUtf8(cameraNames[i])                       ;
        }                                                                    ;
        if (plan->Equipments.contains(N::Plan::DefaultCamera))               {
          AudioDevice = plan->Equipments[N::Plan::DefaultCamera]             ;
        }                                                                    ;
        if (plan->EquipmentIDs.contains(N::Plan::DefaultCamera))             {
          DeviceId = plan->EquipmentIDs[N::Plan::DefaultCamera]              ;
        }                                                                    ;
      } else                                                                 {
        aa  = menu -> addAction  ( QObject::tr("No camera available") )      ;
        aa         -> setEnabled ( false                              )      ;
        return NULL                                                          ;
      }                                                                      ;
    break                                                                    ;
    default                                                                  :
    return NULL                                                              ;
  }                                                                          ;
  ////////////////////////////////////////////////////////////////////////////
  auids = Names.keys ()                                                      ;
  ////////////////////////////////////////////////////////////////////////////
  QActionGroup * g = new QActionGroup(menu)                                  ;
  g->setExclusive(true)                                                      ;
  foreach (uuid,auids)                                                       {
    bool enabled = nEqual ( Names[uuid] , AudioDevice )                      ;
    if ( DeviceId >= 0 ) enabled = nEqual ( uuid , DeviceId )                ;
    QAction * a = g->addAction(Names[uuid])                                  ;
    menu->addAction(a)                                                       ;
    a->setCheckable(true)                                                    ;
    if (enabled) a->setChecked(enabled)                                      ;
    a->setData(uuid)                                                         ;
  }                                                                          ;
  return g                                                                   ;
}
