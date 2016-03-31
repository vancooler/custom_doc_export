<?php

/**
 * @file
 * Theme related functions for processing our output style plugins.
 *
 * Views bug: http://drupal.org/node/593336
 */


/**
 * Theme a status message
 */
function theme_views_data_export_message($var) {
  $output = '';
  $output .= '<div class="messages status ' . $var['type'] . '">';
  $output .= $var['message'];
  $output .= '</div>';
  return $output;
}

/**
 * Theme a feed link.
 *
 * This theme function uses the theme pattern system to allow it to be
 * overidden in a more specific manner. The options for overiding this include
 * providing per display id; per type; per display id and per type.
 *
 * e.g.
 * For the view "export_test" with the display "page_1" and the type "csv" you
 * would have the following options.
 *   views_data_export_feed_icon__export_test__page_1__csv
 *   views_data_export_feed_icon__export_test__page_1
 *   views_data_export_feed_icon__export_test__csv
 *   views_data_export_feed_icon__page_1__csv
 *   views_data_export_feed_icon__page_1
 *   views_data_export_feed_icon__csv
 *   views_data_export_feed_icon
 *
 * @ingroup themeable
 */
function theme_views_data_export_feed_icon($variables) {
  extract($variables, EXTR_SKIP);
  $url_options = array('html' => TRUE);
  if ($query) {
    $url_options['query'] = $query;
  }
  $image = theme('image', array('path' => $image_path, 'alt' => $text, 'title' => $text));
  return l($image, $url, $url_options);
}

/**
 * Theme callback for the export complete page.
 *
 * @param $file
 *  Link to output file
 */
function theme_views_data_export_complete_page($variables) {
  extract($variables, EXTR_SKIP);
  drupal_set_title(t('Data export successful'));
  drupal_add_html_head(array('#tag' => 'meta', '#attributes' => array('http-equiv' =>"Refresh", 'content' => '3;url='. $file)), 'views_data_export_download');
  $output = '';
  $output .= '<p>';
  $output .= t('Your export has been created. View/download the file <a href="@link">here</a> (will automatically download in 3 seconds.)', array('@link' => $file));
  $output .= '</p>';

  if (!empty($return_url)) {
    $output .= '<p>';
    $output .= l(t('Return to previous page'), $return_url);
    $output .= '</p>';
  }
  return $output;
}


function template_preprocess_views_data_export(&$vars) {
  $vars['header'] = $vars['rows']['header'];
  $vars['body'] = $vars['rows']['body'];
  $vars['footer'] = $vars['rows']['footer'];

  $view     = $vars['view'];
  $fields   = &$view->field;
}

function template_preprocess_views_data_export_csv_header(&$vars) {
  _views_data_export_header_shared_preprocess($vars);

  // Make sure we catch saved options that are misspelled. LEGACY
  if (isset($vars['options']['seperator'])) {
    $vars['options']['separator'] = $vars['options']['seperator'];
  }
  // Support old misspelled templates. LEGACY
  $vars['seperator'] =
    $vars['separator'] = $vars['options']['separator'];

  // Special handling when quoted values are involved.
  if ($vars['options']['quote']) {
    $wrap = '"';
    $replace_value = '""';
  }
  else {
    $wrap = '';
    $replace_value = '';
  }

  // Format header values.
  foreach ($vars['header'] as $key => $value) {
    $output = decode_entities($value);
    $output = (empty($vars['options']['keep_html'])) ? strip_tags($output) : $output;
    if (!empty($vars['options']['trim'])) {
      $output = trim($output);
    }
    if (!empty($vars['options']['encoding']) && function_exists('iconv')) {
      switch($vars['options']['encoding']) {
        case 'utf8_decode':
          $converted = utf8_decode($output);
          break;
        default:
          $converted = iconv("UTF-8", $vars['options']['encoding'], $output);
          break;
      }
      if ($converted !== FALSE) {
        $output = $converted;
      }
    }
    $vars['header'][$key] = $wrap . str_replace('"', $replace_value, $output) . $wrap;
  }
}

function template_preprocess_views_data_export_csv_body(&$vars) {
  _views_data_export_body_shared_preprocess($vars);

  // Make sure we catch saved options that are misspelled. LEGACY
  if (isset($vars['options']['seperator'])) {
    $vars['options']['separator'] = $vars['options']['seperator'];
  }
  // Support old misspelled templates. LEGACY
  $vars['seperator'] =
    $vars['separator'] = $vars['options']['separator'];

  // Special handling when quoted values are involved.
  if ($vars['options']['quote']) {
    $wrap = '"';
    $replace_value = '""';
  }
  else {
    $wrap = '';
    $replace_value = '';
  }

  // Format row values.
  foreach ($vars['themed_rows'] as $i => $values) {
    foreach ($values as $j => $value) {
      $output = decode_entities($value);
      $output = (empty($vars['options']['keep_html'])) ? strip_tags($output) : $output;
      if (!empty($vars['options']['trim'])) {
        $output = trim($output);
      }

      if (!empty($vars['options']['encoding']) && function_exists('iconv')) {
        switch($vars['options']['encoding']) {
          case 'utf8_decode':
            $converted = utf8_decode($output);
            break;
          default:
            $converted = iconv("UTF-8", $vars['options']['encoding'], $output);
            break;
        }
        if ($converted !== FALSE) {
          $output = $converted;
        }
      }
      if (!empty($vars['options']['replace_newlines'])) {
        if (!empty($vars['options']['newline_token'])) {
          $output = str_replace( array("\r\n", "\r", "\n"), $vars['options']['newline_replacement'], $output);
        }
        else {
          $output = str_replace("\n", $vars['options']['newline_replacement'], $output);
        }
      }
      $vars['themed_rows'][$i][$j] = $wrap . str_replace('"', $replace_value, $output) . $wrap;
    }
  }
}

/**
 * Preprocess csv output template.
 */
function template_preprocess_views_data_export_csv(&$vars) {
  // TODO Replace items with themed_rows.
  _views_data_export_shared_preprocess($vars);

  // Make sure we catch saved options that are misspelled. LEGACY
  if (isset($vars['options']['separator'])) {
    $vars['options']['separator'] = $vars['options']['seperator'];
  }
  // Support old misspelled templates. LEGACY
  $vars['seperator'] =
    $vars['separator'] = $vars['options']['separator'];

  // Special handling when quoted values are involved.
  if ($vars['options']['quote']) {
    $wrap = '"';
    $replace_value = '""';
  }
  else {
    $wrap = '';
    $replace_value = '';
  }

  // Format header values.
  foreach ($vars['header'] as $key => $value) {
    $output = decode_entities(strip_tags($value));
    if ($vars['options']['trim']) {
      $output = trim($output);
    }
    if (!empty($vars['options']['encoding']) && function_exists('iconv')) {
      switch($vars['options']['encoding']) {
        case 'ASCII':
          $converted = iconv("UTF-8", "ASCII//TRANSLIT", $output);
          if ($converted !== FALSE) {
            $output = $converted;
          }
          break;
      }
    }
    $vars['header'][$key] = $wrap . str_replace('"', $replace_value, $output) . $wrap;
  }

  // Format row values.
  foreach ($vars['themed_rows'] as $i => $values) {
    foreach ($values as $j => $value) {
      $output = decode_entities(strip_tags($value));
      if ($vars['options']['trim']) {
        $output = trim($output);
      }
      if (!empty($vars['options']['encoding']) && function_exists('iconv')) {
        switch($vars['options']['encoding']) {
          case 'ASCII':
            $converted = iconv("UTF-8", "ASCII//TRANSLIT", $output);
            if ($converted !== FALSE) {
              $output = $converted;
            }
            break;
        }
      }
      $vars['themed_rows'][$i][$j] = $wrap . str_replace('"', $replace_value, $output) . $wrap;
    }
  }
}

/**
 * Preprocess txt output template.
 */
function template_preprocess_views_data_export_txt_body(&$vars) {
  _views_data_export_header_shared_preprocess($vars);
  // We support not outputting fields when they are empty, so indicate so.
  $vars['hide_empty_support'] = TRUE;
  _views_data_export_body_shared_preprocess($vars);
}

function template_preprocess_views_data_export_doc_body(&$vars) {
  // Pass through the generic MS Office preprocess.
  template_preprocess_views_data_export_msoffice_body($vars);
}

function template_preprocess_views_data_export_xls_body(&$vars) {
  // Pass through the generic MS Office preprocess.
  template_preprocess_views_data_export_msoffice_body($vars);
}



/**
* function customized to build custom doc template
*
* Author: Yang Liu
*
* Date: 2016-03-28
*
*/
function template_preprocess_views_data_export_msoffice_body(&$vars) {
  _views_data_export_header_shared_preprocess($vars);
  _views_data_export_body_shared_preprocess($vars);

  $output = '';

  // Construct the tbody of a table, see theme_table().

  $ts = tablesort_init($vars['header']);

  $flip = array(
    'even' => 'odd',
    'odd' => 'even',
  );
  $class = 'even';

  /*
  Array
  (
    [0] => Array
        (
            [field_name] => Janine Skerratt
            [field_address] => 33004 Desbrisay Ave
            [field_phone] => 604-217-8028
            [field_city] => Mission
            [field_province] => BC
            [field_postal_code] => V2V 7R6
            [field_education] => 

                  

                    
                June 1995
                Quesnel Senior Secondary
                Quesnel
                BC
                High School Graduate
                Details: 
                Business Management


                  


                , 

                  

                    
                July 1996
                Canadian Travel School
                Vancouver
                BC
                Travel and Tourism
  



            [field_email] => g.spink@telus.net
            [field_employer] => 

                  

                    
                Sept
                current
                Employment Specialists
                Abbotsford
                BC
                Job Title: 
                Restoration and Laborer
                  


                Edit

                Delete

                , 

                  

                    
                Oct 2010
                Oct 2012
                14th AVe Liquor Store
                Mission 
                BC
                Job Title: 
                Night Shift Manager
                  


                Edit

                Delete

                , 

                  

                    
                Sept 2003
                Aug 2011
                Mission Icebreakers Jr Hockey Club
                Mission 
                BC
                Job Title: 
                Owner/Operator
                  


                Edit

                Delete

                , 

                  

                    
                Sept 2010
                Current
                Mission School District 75
                Mission
                BC
                Job Title: 
                Noon Hr Supervisor Assistant
                  


                Edit

                Delete


            [field_interests_activities] => 
            [field_objective] => Customer Service Receptionist
            [field_awards] => 
                Serving It Right

                Food Safe

                Super Host

                Super Host Japan

                Enhanced Drivers Licence Class 5



            [field_skills] => Filing & Data Archiving, Telephone Etique, Customer Service Skills over 10 yrs, Microsoft Word, Excel, Internet & Web Browsing, Typing WPM 40, Management and Supervision
            [field_volunteer] => 
                Mission Minor Hockey



        )

  )
  */
  watchdog("CCC", "<pre>".print_r($output, true)."</pre>");

  if(count($vars['rows']) > 0 and isset($vars['rows'][0]->_field_data['nid']['entity'])){
    $node = $vars['rows'][0]->_field_data['nid']['entity'];
    $name = $node->field_name['und'][0]['value'];
    $city = $node->field_city['und'][0]['value'];
    $address = $node->field_address['und'][0]['value'];
    $phone = $node->field_phone['und'][0]['value'];
    $province = $node->field_province['und'][0]['value'];
    $postal_code = $node->field_postal_code['und'][0]['value'];
    $email = $node->field_email['und'][0]['value'];

    $education = $node->field_education;
    $employer = $node->field_employer;
    $skills = $node->field_skills;

    $education_array = array();
    $employer_array = array();
    $skills_array = array();
    // parse education as array
    if (isset($education['und']) and is_array($education['und']) and count($education['und']) > 0){
      foreach ($education['und'] as $key => $value) {
        $ed_id = $value['value'];
        $education_array[$key] = $value['value'];
      }
    }

    // parse employer as array
    if (isset($employer['und']) and is_array($employer['und']) and count($employer['und']) > 0){
      foreach ($employer['und'] as $key => $value) {
        $em_id = $value['value'];
        $employer_array[$key] = $value['value'];
      }
    }
    // parse skills as array
    if (isset($skills['und']) and is_array($skills['und']) and count($skills['und']) > 0){
      foreach ($skills['und'] as $key => $value) {
        $skills_array[$key] = $value['value'];
      }
    }

    $output = '<div style="margin-bottom:24px">
<p align="center"><strong>'.$name.'</strong><br>
'.$address.'<br>
'.$city.', '.$province.'<br>
'.$phone.' | '.$email.'</p>
</div>
<br>
<div class="header" style="margin-top:30px;margin-bottom:18px;font-weight: bold;border:1px solid #000000; border-collapse:collapse;vertical-align:bottom">Work Experience
</div>

<table style="border:none">
<tr class="row" style="margin-left: -15px;margin-right: -15px;">
<td class="col-sm-2" style="width:20%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">2013 - 2014<br>
Toronto, ON</td>

<td class="col-sm-10" style="width:82%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">Customer Service Assistant | Kelly International
<ul>
  <li>Answered phone and assisted customers with initial inquiries</li>
  <li>Performed basic data entry for online customer database</li>
  <li>Established trusting relationship with customers</li>
</ul>
</td>
</tr>

<tr class="row" style="margin-left: -15px;margin-right: -15px;">
<td class="col-sm-2" style="width:18%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">2010 - 2011<br>
Toronto, ON</td>

<td class="col-sm-10" style="width:82%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">Administrative Assistant Intern | Planning Reactive
<ul>
  <li>Educated new customers by answering product and service questions</li>
  <li>Maintained customer record by updating account information</li>
  <li>Reached out to existing customers for outstanding payments</li>
</ul>
</td>
</tr>

<tr class="row" style="margin-left: -15px;margin-right: -15px;">
<td class="col-sm-2" style="width:18%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">2009 - 2011<br>
Toronto, ON</td>

<td class="col-sm-10" style="width:82%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">Freshman Student Body President | Centennial College
<ul>
  <li>Created agenda for meetings</li>
  <li>Met with students about concerns facing the school</li>
  <li>Organized student social events</li>
</ul>
</td>
</tr>
</table>
<br>
<div class="header" style="margin-top:30px;margin-bottom:18px;font-weight: bold;border:1px solid #000000; border-collapse:collapse;vertical-align:bottom">Education
</div>
<table style="border:none">
<tr class="row" style="margin-left: -15px;margin-right: -15px;">
<td class="col-sm-2" style="width:18%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">2009 - 2013<br>
Toronto, ON</td>

<td class="col-sm-10" style="width:80%;position: relative;min-height: 1px;padding-left: 15px;padding-right: 15px;float: left;">Centennial College<br>
<em>Bartending Certification, Smart Serve Certification</em></td>
</tr>

</table>
<br>
<div class="header" style="margin-top:30px;margin-bottom:18px;font-weight: bold;border:1px solid #000000; border-collapse:collapse;vertical-align:bottom">Additional Skills &amp; Interest
</div>

<table style="border:none">
<tr><td>
<ul>';
    foreach ($skills_array as $key => $value) {
      $output .= '<li>'.$value.'</li>';
    }
    
    $output .= '</ul></td></tr>
    </div>
    </div>';
  }
  watchdog("BBB", "<pre>".print_r($skills_array, true)."</pre>");


  // foreach ($vars['themed_rows'] as $number => $row) {
  //   $attributes = array();

  //   // Check if we're dealing with a simple or complex row
  //   if (isset($row['data'])) {
  //     foreach ($row as $key => $value) {
  //       if ($key == 'data') {
  //         $cells = $value;
  //       }
  //       else {
  //         $attributes[$key] = $value;
  //       }
  //     }
  //   }
  //   else {
  //     $cells = $row;
  //   }
  //   if (count($cells)) {
  //     // Add odd/even class
  //     $class = $flip[$class];
  //     if (isset($attributes['class'])) {
  //       $attributes['class'] .= ' ' . $class;
  //     }
  //     else {
  //       $attributes['class'] = $class;
  //     }

  //     // Build row
  //     $output .= ' <tr' . drupal_attributes($attributes) . '>';
  //     $i = 0;
  //     foreach ($cells as $cell) {
  //       $cell = tablesort_cell($cell, $vars['header'], $ts, $i++);
  //       $output .= _theme_table_cell($cell);
  //     }
  //     $output .= " </tr>\n";
  //   }
  // }


  $vars['tbody'] = preg_replace('/<\/?(a|span) ?.*?>/', '', $output); // strip 'a' and 'span' tags
}

function template_preprocess_views_data_export_doc_header(&$vars) {
  // Pass through the generic MS Office preprocess.
  template_preprocess_views_data_export_msoffice_header($vars);
}

function template_preprocess_views_data_export_xls_header(&$vars) {
  // Pass through the generic MS Office preprocess.
  template_preprocess_views_data_export_msoffice_header($vars);
}

function template_preprocess_views_data_export_msoffice_header(&$vars) {
  _views_data_export_header_shared_preprocess($vars);

  // Need to do a little work to construct the table header, see theme_table().
  $vars['header_row'] = '';
  $vars['header_row'] .= '<thead><tr>';

  $ts = tablesort_init($vars['header']);

  foreach ($vars['header'] as $cell) {
    $cell = tablesort_header($cell, $vars['header'], $ts);
    $vars['header_row'] .= _theme_table_cell($cell, TRUE);
  }

  $vars['header_row'] .= '</tr></thead>';

  $vars['header_row'] = preg_replace('/<\/?(a|span) ?.*?>/', '', $vars['header_row']); // strip 'a' and 'span' tags


  // reset head as empty
  $vars['header_row'] = '<thead><tr></tr></thead>';
}

/**
 * Preprocess xml output template.
 */
function template_preprocess_views_data_export_xml_header(&$vars) {
  $vars['root_node'] = _views_data_export_xml_tag_clean($vars['options']['root_node']);
}

/**
 * Preprocess xml output template.
 */
function template_preprocess_views_data_export_xml_footer(&$vars) {
  $vars['root_node'] = _views_data_export_xml_tag_clean($vars['options']['root_node']);
}

/**
 * Preprocess xml output template.
 */
function template_preprocess_views_data_export_xml_body(&$vars) {
  _views_data_export_header_shared_preprocess($vars);
  // We support not outputting fields when they are empty, so indicate so.
  $vars['hide_empty_support'] = TRUE;
  _views_data_export_body_shared_preprocess($vars);

  $view = $vars['view'];
  $style_options = $view->display_handler->get_option('style_options');

  $no_encode = isset($style_options['no_entity_encode']) ? $style_options['no_entity_encode'] : array();

  $cdata_wrapper = isset($style_options['cdata_wrapper']) ? $style_options['cdata_wrapper'] : array();

  $vars['item_node'] = _views_data_export_xml_tag_clean($vars['options']['item_node']);

  foreach ($vars['themed_rows'] as $num => $row) {
    foreach ($row as $field => $content) {

      // Perform xml entity encoding unless excluded by style options.
      if (empty($no_encode[$field]) && empty($cdata_wrapper[$field])) {

        // Prevent double encoding of the ampersand. Look for the entities produced by check_plain().
        $content = preg_replace('/&(?!(amp|quot|#039|lt|gt);)/', '&amp;', $content);
        // Convert < and > to HTML entities.
        $content = str_replace(
          array('<', '>'),
          array('&lt;', '&gt;'),
          $content);
      }

      // Perform wrapping the field data using the CDATA tag
      // unless excluded by style options.
      if (!empty($cdata_wrapper[$field])) {
        // Escape CDATA end sequence only.
        $content = '<![CDATA[' . str_replace(']]>', ']]]]><![CDATA[>', $content) . ']]>';
      }

      $vars['themed_rows'][$num][$field] = $content;
    }
  }

  foreach ($vars['header'] as $field => $header) {
    // If there is no field label, use 'no name'.
    if (empty($header)) {
      $header = 'no name';
    }
    if ($vars['options']['transform']) {
      switch ($vars['options']['transform_type']) {
        case 'dash':
          $vars['xml_tag'][$field] = str_replace(' ', '-', $header);
          break;
        case 'underline':
          $vars['xml_tag'][$field] = str_replace(' ', '_', $header);
          break;
        case 'camel':
          $vars['xml_tag'][$field] = str_replace(' ', '', ucwords(strtolower($header)));
          // Convert the very first character of the string to lowercase.
          $vars['xml_tag'][$field][0] = strtolower($vars['xml_tag'][$field][0]);
          break;
        case 'pascal':
          $vars['xml_tag'][$field] = str_replace(' ', '', ucwords(strtolower($header)));
          break;
      }
    }
    // We should always try to output valid XML.
    $vars['xml_tag'][$field] = _views_data_export_xml_tag_clean($vars['xml_tag'][$field]);
  }
}

/**
 * Returns a valid XML tag formed from the given input.
 *
 * @param $tag The string that should be made into a valid XML tag.
 * @return The valid XML tag or an empty string if the string contained no valid
 * XML tag characters.
 */
function _views_data_export_xml_tag_clean($tag) {

  // This regex matches characters that are not valid in XML tags, and the
  // unicode ones that are. We don't bother with unicode, because it would so
  // the preg_replace down a lot.
  static $invalid_tag_chars_regex = '#[^\:A-Za-z_\-.0-9]+#';

  // These characters are not valid at the start of an XML tag:
  static $invalid_start_chars = '-.0123456789';

  // Convert invalid chars to '-':
  $tag = preg_replace($invalid_tag_chars_regex, '-', $tag);

  // Need to trim invalid characters from the start of the string:
  $tag = ltrim($tag, $invalid_start_chars);

  // As a last line of defense, if we've stripped out everything, set it to
  // something.
  if (empty($tag)) {
    $tag = 'invalid-tag-name';
  }

  return $tag;
}

/**
 * Shared helper function for export preprocess functions.
 */
function _views_data_export_header_shared_preprocess(&$vars) {
  $view     = $vars['view'];
  $fields   = &$view->field;
  $fields_info = $view->display_handler->get_option('fields');
  $vars['header'] = array();
  foreach ($fields as $key => $field) {
    if (empty($field->options['exclude'])) {
      if (isset($fields_info) && isset($fields_info[$key]['label'])) {
        $vars['header'][$key] = check_plain($fields_info[$key]['label']);
      }
      else {
        $vars['header'][$key] = check_plain($field->label());
      }
    }
  }
}

/**
 * Shared helper function for export preprocess functions.
 */
function _views_data_export_body_shared_preprocess(&$vars) {
  $view     = $vars['view'];
  $fields   = &$view->field;
  $hide_empty_support = !empty($vars['hide_empty_support']);

  $rows = $vars['rows'];

  $vars['themed_rows'] = array();
  $keys = array_keys($fields);
  foreach ($rows as $num => $row) {
    $vars['themed_rows'][$num] = array();

    foreach ($keys as $id) {
      if (empty($fields[$id]->options['exclude'])) {
        $content = $view->style_plugin->rendered_fields[$num][$id];

        if ($hide_empty_support && !empty($fields[$id]->options['hide_empty'])) {
          if ($fields[$id]->is_value_empty($content, $fields[$id]->options['empty_zero'])) {
            continue;
          }
        }

        $vars['themed_rows'][$num][$id] = $content;
      }
    }
  }
}
