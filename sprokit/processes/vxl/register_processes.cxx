// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

#include <sprokit/processes/vxl/kwiver_processes_vxl_export.h>

#include <sprokit/pipeline/process_factory.h>
#include <vital/plugin_management/plugin_loader.h>

// -- list processes to register --
#include "kw_archive_writer_process.h"

// ----------------------------------------------------------------
/*! \brief Regsiter processes
 *
 */
extern "C"
KWIVER_PROCESSES_VXL_EXPORT
void register_factories( kwiver::vital::plugin_loader& vpm )
{
  using namespace sprokit;

  process_registrar reg( vpm, "kwiver_processes_vxl" );

  if ( reg.is_module_loaded() )
  {
    return;
  }

  reg.register_process< kwiver::kw_archive_writer_process >();

  // - - - - - - - - - - - - - - - - - - - - - - -
  reg.mark_module_as_loaded();

}
