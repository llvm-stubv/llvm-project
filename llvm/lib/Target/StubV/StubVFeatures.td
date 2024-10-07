//===-- StubVFeatures.td - Stub-V Features and Extensions --*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Stub-V subtarget features and instruction predicates.
//===----------------------------------------------------------------------===//

// Subclass of SubtargetFeature to be used when the feature is also a Stub-V
// extension. Extensions have a version and may be experimental.
//
// name      - Name of the extension in lower case.
// major     - Major version of extension.
// minor     - Minor version of extension.
// desc      - Description of extension.
// implies   - Extensions or features implied by this extension.
// fieldname - name of field to create in StubVSubtarget. By default replaces
//             uses the record name by replacing Feature with Has.
// value     - Value to assign to the field in StubVSubtarget when this
//             extension is enabled. Usually "true", but can be changed.
class StubVExtension<string name, int major, int minor, string desc,
                     list<SubtargetFeature> implies = [],
                     string fieldname = !subst("Feature", "Has", NAME),
                     string value = "true">
    : SubtargetFeature<name, fieldname, value, desc, implies> {
  // MajorVersion - The major version for this extension.
  int MajorVersion = major;

  // MinorVersion - The minor version for this extension.
  int MinorVersion = minor;

  // Experimental - Does extension require -menable-experimental-extensions.
  bit Experimental = false;
}

// Multiply Extensions

def FeatureStdExtZmmul
    : StubVExtension<"zmmul", 1, 0,
                     "'Zmmul' (Integer Multiplication)">;
def HasStdExtZmmul : Predicate<"Subtarget->hasStdExtZmmul()">,
                     AssemblerPredicate<(all_of FeatureStdExtZmmul),
                     "'Zmmul' (Integer Multiplication)">;

def FeatureStdExtM
    : StubVExtension<"m", 2, 0,
                     "'M' (Integer Multiplication and Division)",
                     [FeatureStdExtZmmul]>;
def HasStdExtM : Predicate<"Subtarget->hasStdExtM()">,
                 AssemblerPredicate<(all_of FeatureStdExtM),
                     "'M' (Integer Multiplication and Division)">;
